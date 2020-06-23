Texture2D shadertexture:register(t0);

SamplerState SampleTypeWrap:register(s0);

struct PixelInputType
{
	float4 position:SV_POSITION;
	float2 tex:TEXCOORD0;
	float3 normal:NORMAL;
};

struct PixelOutputType
{
	float4 color:SV_Target0;
	float4 normal : SV_Target1;
};

PixelOutputType main(PixelInputType Input) : SV_TARGET
{
	PixelOutputType output;

	output.color = shadertexture.Sample(SampleTypeWrap, Input.tex);

	output.normal = float4(Input.normal, 1.0F);


	return output;
}