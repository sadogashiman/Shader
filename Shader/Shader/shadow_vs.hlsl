
cbuffer MatrixBuffer
{
    matrix world;
    matrix view;
    matrix projection;
    matrix lightView;
    matrix lightProjection;
};

cbuffer LightBuffer2
{
    float3 lightposition;
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
    float3 lightpos : TEXCOORD2;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    float4 worldposition;
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
    
    worldposition = mul(input.position, world);
    
    output.lightpos = lightposition.xyz - worldposition.xyz;
    
    output.lightpos = normalize(output.lightpos);

    return output;
}