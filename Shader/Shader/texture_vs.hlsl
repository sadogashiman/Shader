cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position:POSITION;
	float2 tex:TEXCOORD0;
};

struct PixelInputType
{
	float4 position:SV_POSITION;
	float2 tex:TEXCOORD0;
};

PixelInputType main(VertexInputType Input)
{
	PixelInputType output;

	//�v�Z�̂��߂�1����
	Input.position.w = 1.0F;

	//���_���W���v�Z
	output.position = mul(Input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//�e�N�X�`����ۑ�
	output.tex = Input.tex;

	return output;
}