Texture2D ShaderTexture;
SamplerState SampleType;

struct PixelInputType
{
	float4 position:SV_POSITION;
	float2 tex:TEXCOORD0;
};

float4 main(PixelInputType Input) :SV_TARGET
{
	float4 textureColor;

	//テクスチャからピクセルカラーをサンプリング
	textureColor = ShaderTexture.Sample(SampleType, Input.tex);

	return textureColor;
}