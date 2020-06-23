cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VertexInputType
{
	float4 position : POSITION;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 domposition : TEXCOORD0;
};

PixelInputType main( VertexInputType Input )
{
	PixelInputType output;
	//4�P�ʂɕϊ�
	Input.position.w = 1.0F;
	
	//���_���W���Z�o
	output.position = mul(Input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);
	
	output.domposition = Input.position;
	
	return output;
}