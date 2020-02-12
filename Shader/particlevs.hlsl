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
	float4 color:COLOR;
};

struct PixelInputType
{
	float4 position:POSITION;
	float2 tex:TEXCOORD;
	float4 color:COLOR;
};

PixelInputType main(VertexInputType Input)
{
	PixelInputType output;

	//位置ベクトルを4単位に変換
	Input.position.w = 1.0F;

	//各行列に対する頂点の位置を計算
	output.position = mul(Input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	//テクスチャを保存
	output.tex = Input.tex;

	//色情報を保存
	output.color = Input.color;

	return output;
}