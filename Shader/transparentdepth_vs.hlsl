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
	float4 depthposition:TEXCOORD0;
	float2 tex:TEXCOORD1;
};

PixelInputType main(VertexInputType Input)
{
	PixelInputType output;

	//�v�Z�̂��߂ɑ��
	Input.position.w = 1.0F;
	
	//���_���W���v�Z
	output.position = mul(Input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	//�[�x�l��ۑ�
	output.depthposition = output.position;

	//�e�N�X�`����ۑ�
	output.tex = Input.tex;

	return output;

}


