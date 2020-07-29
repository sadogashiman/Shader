cbuffer MatrixBuffer : register(b0)
{
    matrix projection;
    matrix world;
    matrix view;
}

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldpos : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
};

PixelInputType main(VertexInputType Input)
{
    PixelInputType output;
    
    Input.position.w = 1.0F;
    
    //���_���W���v�Z
    output.position = mul(Input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    
    output.worldpos = mul(Input.position, world);
    
    //�@�����v�Z
    output.normal = mul(float4(Input.normal, 1.0F), world);
    
    //�@���𐳋K��
    output.normal = normalize(output.normal);
    
    //�e�N�X�`�����W��ۑ�
    output.tex = Input.tex;
    
    return output;
}