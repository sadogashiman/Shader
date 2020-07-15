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
};

struct PixelInputType
{
	float4 position:SV_POSITION;
	float2 tex:TEXCOORD;
};

PixelInputType main(VertexInputType Input)
{
	PixelInputType output;

	//�ʒu�x�N�g����ϊ�
	Input.position.w = 1.0F;

	//���[���h�A�r���[���e�}�g���b�N�X�ɑ΂��钸�_�̈ʒu���v�Z
	output.position = mul(Input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	//�s�N�Z���V�F�[�_�̃e�N�X�`�����W��ۑ�
	output.tex = Input.tex;

	return output;
}