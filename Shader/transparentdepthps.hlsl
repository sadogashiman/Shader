Texture2D shadertexture:register(s0);

SamplerState sampletype:register(s1);

struct PixelInputType
{
	float4 position:SV_POSITION;
	float4 depthposition:TEXCOORD0;
	float2 tex:TEXCOORD1;
};

float4 main(PixelInputType Input) :SV_TARGET
{
	float depthvalue;
	float4 color;
	float4 texturecolor;

	texturecolor = shadertexture.Sample(sampletype, Input.tex);

	if (texturecolor.a > 0.8F)
	{
		depthvalue = Input.depthposition.z / Input.depthposition.w;
	}
	else
	{
		discard;
	}

	color = float4(depthvalue, depthvalue, depthvalue, 1.0F);

	return color;
}