cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

PixelInputType main(VertexInputType input)
{
	PixelInputType output;

	//ベクトルを4単位に変換
	input.position.w = 1.0f;

	//頂点座標を計算
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//テクスチャ座標を保存
	output.tex = input.tex;
	
	//ワールド行列にのみ法線を計算
    output.normal = mul(input.normal, (float3x3) worldMatrix);
	
	//法線を正規化
    output.normal = normalize(output.normal);

	return output;
}