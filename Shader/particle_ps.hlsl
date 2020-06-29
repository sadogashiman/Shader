Texture2D shadertexture;
SamplerState sampletype : register(s0);

struct PixelInputType
{
	float4 position:SV_POSITION;
	float2 tex:TEXCOORD;
	float4 color:COLOR;
};

float4 main(PixelInputType Input) :SV_TARGET
{
	float4 texturecolor;
	float4 finalcolor;

	//テクスチャからピクセルカラーをサンプリング
	texturecolor = shadertexture.Sample(sampletype, Input.tex);

	//最終的なカラーを決定
	finalcolor = texturecolor * Input.color;


	return finalcolor;

}