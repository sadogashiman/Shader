Texture2D shadertexture;
SamplerState sampletype;

cbuffer LightBuffer
{
    float4 ambient;
    float4 diffuse;
    float3 lightdirection;
    float padding;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

float4 main(PixelInputType input):SV_TARGET
{
    float4 texturecolor;
    float3 lightdir;
    float lightintensity;
    float4 color;
    
    //�e�N�X�`���T���v�����O
    texturecolor = shadertexture.Sample(sampletype, input.tex);
    
    //������ݒ�
    color = ambient;
    
    //�v�Z�̂��߂Ƀ��C�g�����𔽓]
    lightdir = -lightdirection;
    
    //���ʂ��v�Z
    lightintensity = saturate(dot(input.normal, lightdir));
    
    if(lightintensity>0.0F)
    {
        //�g�U�F�ƌ��ʂɊ�Â��Ċg�U�F������
        color += (diffuse * lightintensity);
    }
    
    //�F��O�a
    color = saturate(color);
    
    color = color * texturecolor;
    
    return color;
    
}