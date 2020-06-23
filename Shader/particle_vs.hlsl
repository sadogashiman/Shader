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
	float4 color:COLOR;
};

struct PixelInputType
{
	float4 position:POSITION;
	float2 tex:TEXCOORD;
	float4 color:COLOR;
};

PixelInputType main(VertexInputType Input)
{
	PixelInputType output;

	//�ʒu�x�N�g����4�P�ʂɕϊ�
	Input.position.w = 1.0F;

	//�e�s��ɑ΂��钸�_�̈ʒu���v�Z
	output.position = mul(Input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	//�e�N�X�`����ۑ�
	output.tex = Input.tex;

	//�F����ۑ�
	output.color = Input.color;

	return output;
}