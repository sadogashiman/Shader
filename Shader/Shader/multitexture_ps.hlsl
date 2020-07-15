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

	//�ŏ��̃e�N�X�`������s�N�Z���J���[���擾
	color1 = shadertexture[0].Sample(sampletype, Input.tex);

	//��Ԗڂ̃e�N�X�`������s�N�Z���J���[���擾
	color2 = shadertexture[1].Sample(sampletype, Input.tex);

	//��̃s�N�Z�����u�����h���ăK���}�l����Z
	blendcolor = color1 * color2 * 2.0F;

	//�ŏI�F���ɘa
	blendcolor = saturate(blendcolor);

	return blendcolor;
}