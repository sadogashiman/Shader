cbuffer Matrixbuffer
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VertexInputType
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 color : COLOR0;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;
	
	//ベクトルを4単位に変換
	input.position.w = 1.0F;
	
	//頂点座標位置を計算
	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, projection);
	
	//出力色を設定
	output.color = input.color;
	
	return output;
}