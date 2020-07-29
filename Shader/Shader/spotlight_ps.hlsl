Texture2D shadertexture : register(t0);
SamplerState samplerstate : register(s0);

cbuffer LightBuffer : register(b0)
{
    float3 lightpos;
    float range;
    float4 diffusecolor;
    float4 ambientcolor;
    float3 lightdir;
    float cone;
    float3 att;
    float padding;
}

struct PixelInputType
{
    float4 position : SV_Position;
    float4 worldpos : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(PixelInputType Input) : SV_TARGET
{
    float4 color = { 0.0F, 0.0F, 0.0F, 1.0F };
    float3 pixelvec;
    float4 diffuse = diffusecolor;
    diffuse = diffuse * shadertexture.Sample(samplerstate, Input.tex);
    
    //ピクセルのベクトルを計算
    pixelvec = lightpos - Input.worldpos.xyz;
    
    //ライトとピクセルの距離を算出
    float d = length(pixelvec);
    
    //アンビエント光を追加
    float4 ambient = diffuse * ambientcolor;
    
    //ピクセルが範囲外の場合アンビエントだけを適応
    if (d > range)
        return diffuse;
    
    //単位長のベクトルに変換
    pixelvec /= d;
    
    //ピクセルの光量を計算
    float lightintencity = dot(pixelvec, Input.normal);
    
    //照らされるピクセルの場合
    if (lightintencity > 0.0F)
    {
        //ディフューズライトを追加
        color += diffuse;
        
        //距離減衰を計算
        color /= (att[0] + att[1] * d) + (att[2] * (d * d));
        
        //エッジの減衰を計算
        color *= pow(max(dot(-pixelvec, lightdir), 0.0F), cone);
    }
    
    //値が1~0の間にあるか確認
    color = saturate(color + ambient);
    
    return color;
}