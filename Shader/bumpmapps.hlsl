Texture2D shaderTextures[2];
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 diffuseColor;
    float3 lightDirection;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float4 bumpMap;
    float3 bumpNormal;
    float3 lightDir;
    float lightIntensity;
    float4 color;

    //テクスチャからサンプリング
    textureColor = shaderTextures[0].Sample(SampleType, input.tex);

    //バンプマップでピクセルをサンプリング
    bumpMap = shaderTextures[1].Sample(SampleType, input.tex);

    //値の範囲を拡大
    bumpMap = (bumpMap * 2.0f) - 1.0f;

    //バンプマップのデータから法線を計算
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);

    //計算結果の法線を正規化
    bumpNormal = normalize(bumpNormal);

    //計算のためにライトを反転
    lightDir = -lightDirection;

    //バンプマップの法線に基づいて光量を計算
    lightIntensity = saturate(dot(bumpNormal, lightDir));

    //拡散色と光の強度に基づいて最終的な拡散色を決定
    color = saturate(diffuseColor * lightIntensity);

    //最終的なバンプライトカラーとテクスチャカラーを組み合わせる
    color = color * textureColor;

    return color;
}
