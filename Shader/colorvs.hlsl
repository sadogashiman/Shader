cbuffer Matrixbuffer
{
    matrix world;
    matrix view;
    matrix projection;
};

struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
    //�x�N�g����4�P�ʂɕϊ�
    input.position.w = 1.0F;
    
    //���_���W�ʒu���v�Z
    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    
    //�o�͐F��ݒ�
    output.color = input.color;
    
	return output;
}