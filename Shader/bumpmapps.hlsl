Texture2D shaderTextures[2];
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 diffuseColor;
    float3 lightDirection;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor;
    float4 bumpMap;
    float3 bumpNormal;
    float3 lightDir;
    float lightIntensity;
    float4 color;

    //�e�N�X�`������T���v�����O
    textureColor = shaderTextures[0].Sample(SampleType, input.tex);

    //�o���v�}�b�v�Ńs�N�Z�����T���v�����O
    bumpMap = shaderTextures[1].Sample(SampleType, input.tex);

    //�l�͈̔͂��g��
    bumpMap = (bumpMap * 2.0f) - 1.0f;

    //�o���v�}�b�v�̃f�[�^����@�����v�Z
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);

    //�v�Z���ʂ̖@���𐳋K��
    bumpNormal = normalize(bumpNormal);

    //�v�Z�̂��߂Ƀ��C�g�𔽓]
    lightDir = -lightDirection;

    //�o���v�}�b�v�̖@���Ɋ�Â��Č��ʂ��v�Z
    lightIntensity = saturate(dot(bumpNormal, lightDir));

    //�g�U�F�ƌ��̋��x�Ɋ�Â��čŏI�I�Ȋg�U�F������
    color = saturate(diffuseColor * lightIntensity);

    //�ŏI�I�ȃo���v���C�g�J���[�ƃe�N�X�`���J���[��g�ݍ��킹��
    color = color * textureColor;

    return color;
}
