Texture2D shadertexture[2];
SamplerState sampletype;

struct PixelInputType
{
	float4  position:SV_POSITION;
	float2 tex:TEXCOORD;
};

float4 main(PixelInputType Input) :SV_TARGET
{
	float4 color1;
	float4 color2;
	float4 blendcolor;

	//最初のテクスチャからピクセルカラーを取得
	color1 = shadertexture[0].Sample(sampletype, Input.tex);

	//二番目のテクスチャからピクセルカラーを取得
	color2 = shadertexture[1].Sample(sampletype, Input.tex);

	//二つのピクセルをブレンドしてガンマ値を乗算
	blendcolor = color1 * color2 * 2.0F;

	//最終色を緩和
	blendcolor = saturate(blendcolor);

	return blendcolor;
}