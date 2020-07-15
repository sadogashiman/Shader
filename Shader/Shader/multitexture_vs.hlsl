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

	//位置ベクトルを変換
	Input.position.w = 1.0F;

	//ワールド、ビュー投影マトリックスに対する頂点の位置を計算
	output.position = mul(Input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);

	//ピクセルシェーダのテクスチャ座標を保存
	output.tex = Input.tex;

	return output;
}