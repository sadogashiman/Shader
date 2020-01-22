Texture2D shadertexture[3];
SamplerState samplertype;

struct PixelInputType
{
    float4 position:SV_POSITION;
    float2 tex:TEXCOORD;
};

float4 main(PixelInputType Input) : SV_TARGET
{
    float4 color1;
    float4 color2;
    float4 alphavalue;
    float4 blendcolor;

    //�e�N�X�`������s�N�Z���J���[�擾
    color1 = shadertexture[0].Sample(samplertype, Input.tex);
    color2 = shadertexture[1].Sample(samplertype, Input.tex);

    //�A���t�@�}�b�v�e�N�X�`������A���t�@�l���擾
    alphavalue = shadertexture[2].Sample(samplertype, Input.tex);

    //�A���t�@�l�Ɋ�Â��ē�̃e�N�X�`����g�ݍ��킹��
    blendcolor = (alphavalue*color1) + ((1.0 - alphavalue)*color2);

    //�F�̊ɘa
    blendcolor = saturate(blendcolor);

    return blendcolor;
}