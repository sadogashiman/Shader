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
	float3 normal:NORMAL;
};

struct PixelInputType
{
	float4 position:SV_POSITION;
	float2 tex:TEXCOORD0;
	float3 normal:NORMAL;
};


PixelInputType main(VertexInputType Input)
{
	PixelInputType output;

	Input.position.w = 1.0F;

	output.position = mul(Input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	output.tex = Input.tex;

	output.normal = mul(Input.normal, (float3x3)world);

	output.normal = normalize(output.normal);


	return output;
}