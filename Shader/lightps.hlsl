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

	//�|�C���g�T���v���[���g�p���ăJ���[�����_�����O�e�N�X�`������F���T���v�����O
	colors = colorTexture.Sample(SampleTypePoint, input.tex);

	//�v�Z�̂��߂Ƀ��C�g�̕����𔽓]
	lightDir = -lightDirection;

	//���̃s�N�Z���̌��ʂ��v�Z
	lightIntensity = saturate(dot(input.normal, lightDir));

	//�s�N�Z���̐F�ƌ��̋��x��g�ݍ��킹�čŏI�I�Ȋg�U�F������
	outputColor = saturate(colors + lightIntensity);

	//�ŏI�I�ȃs�N�Z���J���[���擾
	outputColor = outputColor * colors;
	
	return outputColor;
}
