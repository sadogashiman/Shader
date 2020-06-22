Texture2D cloud1 : register(t0);
Texture2D cloud2 : register(t1);
SamplerState sampletype;

cbuffer SkyBuffer
{
    float firsttransx;
    float firsttransz;
    float secondtransx;
    float secondtransz;
    float bright;
    float3 padding;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float2 samplelocation;
    float4 texcolor1;
    float4 texcolor2;
    float4 finalcolor;
    
    //ピクセルサンプリングの座標を計算
    samplelocation.x = input.tex.x + firsttransx;
    samplelocation.y = input.tex.y + firsttransz;
    
    //サンプリング
    texcolor1 = cloud1.Sample(sampletype, samplelocation);
    
    //2番目のサンプリング
    samplelocation.x = input.tex.x + secondtransx;
    samplelocation.y = input.tex.y + secondtransz;
    
    //サンプリング
    texcolor2 = cloud2.Sample(sampletype, samplelocation);
    
    //二つの画像を線形補間で補間する
    finalcolor = lerp(texcolor1, texcolor2, 0.5F);
    
    //明るさを乗算
    finalcolor = finalcolor * bright;
    
    return finalcolor;
}