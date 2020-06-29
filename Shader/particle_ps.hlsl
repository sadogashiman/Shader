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

	//�e�N�X�`������s�N�Z���J���[���T���v�����O
	texturecolor = shadertexture.Sample(sampletype, Input.tex);

	//�ŏI�I�ȃJ���[������
	finalcolor = texturecolor * Input.color;


	return finalcolor;

}