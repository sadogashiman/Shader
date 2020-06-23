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

	Input.position.w = 1.0F;

	output.position = mul(Input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	output.depthposition = output.position;

	output.tex = Input.tex;

	return output;

}


