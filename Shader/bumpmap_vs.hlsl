cbuffer MatrixBuffer
{
    matrix world;
    matrix view;
    matrix projection;
};

struct VertexInputType
{
    float4 position:POSITION;
    float2 tex:TEXCOORD;
    float3 normal:NORMAL;
    float3 tangent:TANGENT;
    float3 binormal:BINORMAL;
};

struct PixelInputType
{
    float4 position:SV_POSITION;
    float2 tex:TEXCOORD;
    float3 normal:NORMAL;
    float3 tangent :TANGENT;
    float3 binormal:BINORMAL;
};

PixelInputType main(VertexInputType Input)
{
    PixelInputType output;

    Input.position.w = 1.0F;

    output.position = mul(Input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);

    output.tex = Input.tex;

    //���[���h�}�g���b�N�X�݂̂ɑ΂��Ė@���x�N�g�����v�Z���čŏI�I�Ȓl�𐳋K��
    output.normal = mul(Input.normal, (float3x3)world);
    output.normal = normalize(output.normal);

    //���[���h�}�g���b�N�X�݂̂ɑ΂��Đڐ��x�N�g�����v�Z���čŏI�I�Ȓl�𐳋K��
    output.tangent = mul(Input.tangent, (float3x3)world);
    output.tangent = normalize(output.tangent);

    //���[���h�}�g���b�N�X�݂̂ɑ΂��ď]�@���x�N�g�����v�Z���čŏI�I�Ȓl�𐳋K��
    output.binormal = mul(Input.binormal, (float3x3)world);
    output.binormal = normalize(output.binormal);

    return output;
}
