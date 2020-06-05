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
	//4単位に変換
	Input.position.w = 1.0F;
	
	//頂点座標を算出
	output.position = mul(Input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);
	
	output.domposition = Input.position;
	
	return output;
}