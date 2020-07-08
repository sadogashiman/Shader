
cbuffer MatrixBuffer
{
    matrix world;
    matrix view;
    matrix projection;
    matrix lightView;
    matrix lightProjection;
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
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    input.position.w = 1.0f;
    
    //���_���W���v�Z
    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);

    //�J�������猩�����W���v�Z
    output.lightViewPosition = mul(input.position, world);
    output.lightViewPosition = mul(output.lightViewPosition, lightView);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjection);

    //�e�N�X�`����ۑ�
    output.tex = input.tex;

    //���[���h�s��ɂ̂ݖ@����K��
    output.normal = mul(input.normal, (float3x3)world);

    //�@���𐳋K��
    output.normal = normalize(output.normal);

    return output;
}