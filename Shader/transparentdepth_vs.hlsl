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

	//計算のために代入
	Input.position.w = 1.0F;
	
	//頂点座標を計算
	output.position = mul(Input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	//深度値を保存
	output.depthposition = output.position;

	//テクスチャを保存
	output.tex = Input.tex;

	return output;

}


