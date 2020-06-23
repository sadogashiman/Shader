Texture2D cloud : register(t0);
Texture2D perturb : register(t1);
SamplerState sampletype;

cbuffer SkyBuffer
{
    float translation;
    float scale;
    float bright;
    float padding;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 perturbvalue;
    float4 cloudcolor;
    
    //移動量でサンプル場所を変える
    input.tex.x = input.tex.x + translation;
   
    //移動後の座標でサンプリング
    perturbvalue = perturb.Sample(sampletype, input.tex);
    
    //スケールを適応
    perturbvalue = perturbvalue * scale;
    
    //サンプリング位置を決定
    perturbvalue.xy = perturbvalue.xy + input.tex.xy + translation;
    
    //雲をサンプリング
    cloudcolor = cloud.Sample(sampletype, perturbvalue.xy);
    
    //明るさを乗算
    cloudcolor = cloudcolor * bright;
    
    return cloudcolor;
}