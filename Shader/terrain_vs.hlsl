cbuffer Matrixbuffer
{
    matrix world;
    matrix view;
    matrix projection;
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
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
    //�v�Z�̂��߂ɑ��
    input.position.w = 1.0F;
    
    //���_���W���v�Z
    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    
    //�e�N�X�`��UV��ۑ�
    output.tex = input.tex;
    
    //�@���Ƀ��[���h�s��̂݌v�Z��������
    output.normal = mul(input.normal, (float3x3) world);
    
    //�@���𐳋K��
    output.normal = normalize(output.normal);
    
    //�o��
    return output;
}