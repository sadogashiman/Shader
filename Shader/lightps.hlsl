Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);

SamplerState SampleTypePoint : register(s0);

cbuffer LightBuffer
{
	float3 lightDirection;
	float padding;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float4 colors;
	float4 normals;
	float3 lightDir;
	float lightIntensity;
	float4 outputColor;

	//ポイントサンプラーを使用してカラーレンダリングテクスチャから色をサンプリング
	colors = colorTexture.Sample(SampleTypePoint, input.tex);

	//計算のためにライトの方向を反転
	lightDir = -lightDirection;

	//このピクセルの光量を計算
	lightIntensity = saturate(dot(input.normal, lightDir));

	//ピクセルの色と光の強度を組み合わせて最終的な拡散色を決定
	outputColor = saturate(colors + lightIntensity);

	//最終的なピクセルカラーを取得
	outputColor = outputColor * colors;
	
	return outputColor;
}
