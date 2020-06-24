
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
};

cbuffer LightBuffer
{
    float3 lightPosition;
    float padding;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;
    float3 lightPos : TEXCOORD2;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;

    input.position.w = 1.0f;
    
    //���_���W���v�Z
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    //�J�������猩�����W���v�Z
    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

    //�e�N�X�`����ۑ�
    output.tex = input.tex;

    //���[���h�s��ɂ̂ݖ@����K��
    output.normal = mul(input.normal, (float3x3)worldMatrix);

    //�@���𐳋K��
    output.normal = normalize(output.normal);

    //���[���h�s��ƍ��W���v�Z
    worldPosition = mul(input.position, worldMatrix);

    //���C�g���W���v�Z
    output.lightPos = lightPosition.xyz - worldPosition.xyz;

    //���C�g���W�𐳋K��
    output.lightPos = normalize(output.lightPos);

    return output;
}