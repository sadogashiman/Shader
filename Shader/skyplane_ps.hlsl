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
    
    //�ړ��ʂŃT���v���ꏊ��ς���
    input.tex.x = input.tex.x + translation;
   
    //�ړ���̍��W�ŃT���v�����O
    perturbvalue = perturb.Sample(sampletype, input.tex);
    
    //�X�P�[����K��
    perturbvalue = perturbvalue * scale;
    
    //�T���v�����O�ʒu������
    perturbvalue.xy = perturbvalue.xy + input.tex.xy + translation;
    
    //�_���T���v�����O
    cloudcolor = cloud.Sample(sampletype, perturbvalue.xy);
    
    //���邳����Z
    cloudcolor = cloudcolor * bright;
    
    return cloudcolor;
}