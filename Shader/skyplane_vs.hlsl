cbuffer MatrixBuffer
{
    matrix world;
    matrix view;
    matrix projection;
};

struct VertexInputType
{
    float4 position:POSITION;
    float2 tex : TEXCOORD;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

PixelInputType main( VertexInputType input )
{
    
    PixelInputType output;
    
    //�v�Z�̂��߂�1����
    input.position.w = 1.0F;
    
    //���W���v�Z
    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    
    //�e�N�X�`����ۑ�
    output.tex = input.tex;
    
	return output;
}