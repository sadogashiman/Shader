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
    
    //�s�N�Z���̃x�N�g�����v�Z
    pixelvec = lightpos - Input.worldpos.xyz;
    
    //���C�g�ƃs�N�Z���̋������Z�o
    float d = length(pixelvec);
    
    //�A���r�G���g����ǉ�
    float4 ambient = diffuse * ambientcolor;
    
    //�s�N�Z�����͈͊O�̏ꍇ�A���r�G���g������K��
    if (d > range)
        return diffuse;
    
    //�P�ʒ��̃x�N�g���ɕϊ�
    pixelvec /= d;
    
    //�s�N�Z���̌��ʂ��v�Z
    float lightintencity = dot(pixelvec, Input.normal);
    
    //�Ƃ炳���s�N�Z���̏ꍇ
    if (lightintencity > 0.0F)
    {
        //�f�B�t���[�Y���C�g��ǉ�
        color += diffuse;
        
        //�����������v�Z
        color /= (att[0] + att[1] * d) + (att[2] * (d * d));
        
        //�G�b�W�̌������v�Z
        color *= pow(max(dot(-pixelvec, lightdir), 0.0F), cone);
    }
    
    //�l��1~0�̊Ԃɂ��邩�m�F
    color = saturate(color + ambient);
    
    return color;
}