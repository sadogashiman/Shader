Texture2D shadertexture : register(t0);
SamplerState samplestate : register(s0);

struct PixelInputType
{
    float4 Pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PixelInputType Input) : SV_TARGET
{
    //正常に計算するために1を代入
    Input.Pos.w = 1.0F;
   
    //サンプラーを適応して戻す
    return shadertexture.Sample(samplestate, Input.tex);
}