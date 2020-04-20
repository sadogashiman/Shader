Texture2D shadertexture : register(t0);
SamplerState samplestate : register(s0);

struct PixelInputType
{
    float4 Pos : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(PixelInputType Input) : SV_TARGET
{
    //����Ɍv�Z���邽�߂�1����
    Input.Pos.w = 1.0F;
   
    //�T���v���[��K�����Ė߂�
    return shadertexture.Sample(samplestate, Input.tex);
}