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
    
    //�s�N�Z���T���v�����O�̍��W���v�Z
    samplelocation.x = input.tex.x + firsttransx;
    samplelocation.y = input.tex.y + firsttransz;
    
    //�T���v�����O
    texcolor1 = cloud1.Sample(sampletype, samplelocation);
    
    //2�Ԗڂ̃T���v�����O
    samplelocation.x = input.tex.x + secondtransx;
    samplelocation.y = input.tex.y + secondtransz;
    
    //�T���v�����O
    texcolor2 = cloud2.Sample(sampletype, samplelocation);
    
    //��̉摜����`��Ԃŕ�Ԃ���
    finalcolor = lerp(texcolor1, texcolor2, 0.5F);
    
    //���邳����Z
    finalcolor = finalcolor * bright;
    
    return finalcolor;
}