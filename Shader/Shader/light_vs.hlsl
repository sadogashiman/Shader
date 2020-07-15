cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
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

	//�x�N�g����4�P�ʂɕϊ�
	input.position.w = 1.0f;

	//���_���W���v�Z
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//�e�N�X�`�����W��ۑ�
	output.tex = input.tex;
	
	//���[���h�s��ɂ̂ݖ@�����v�Z
    output.normal = mul(input.normal, (float3x3) worldMatrix);
	
	//�@���𐳋K��
    output.normal = normalize(output.normal);

	return output;
}