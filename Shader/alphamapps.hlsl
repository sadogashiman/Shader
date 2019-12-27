Texture2D shadertexture[3];
SamplerState samplertype;

struct PixelInputType
{
    float4 position:SV_POSITION;
    float2 tex:TEXCOORD;
};

float4 main(PixelInputType Input) : SV_TARGET
{
    float4 color1;
    float4 color2;
    float4 alphavalue;
    float4 blendcolor;

    //テクスチャからピクセルカラー取得
    color1 = shadertexture[0].Sample(samplertype, Input.tex);
    color2 = shadertexture[1].Sample(samplertype, Input.tex);

    //アルファマップテクスチャからアルファ値を取得
    alphavalue = shadertexture[2].Sample(samplertype, Input.tex);

    //アルファ値に基づいて二つのテクスチャを組み合わせる
    blendcolor = (alphavalue*color1) + ((1.0 - alphavalue)*color2);

    //色の緩和
    blendcolor = saturate(blendcolor);

    return blendcolor;
}