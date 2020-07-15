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

	//計算のために1を代入
	Input.position.w = 1.0F;

	//頂点座標を計算
	output.position = mul(Input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//テクスチャを保存
	output.tex = Input.tex;

	return output;
}