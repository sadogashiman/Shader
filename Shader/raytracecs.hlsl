//
//
struct Buff
{
	float3 E;
	float3 V;
};

struct BuffOut
{
	uint color;
};

struct RET
{
	float3 P;
	float3 N;
	float d;
};

cbuffer global : register(b0)
{
	float3 g_Pos : packoffset(c0); //���S���W
	float g_Radius : packoffset(c1); //���a
	matrix g_mSphereIW : packoffset(c2);
	matrix g_mIV : packoffset(c6);
	float3 g_LightPos : packoffset(c10);
};

StructuredBuffer<Buff> BufferInput : register(t0);
RWStructuredBuffer<BuffOut> BufferOut : register(u0);
//
//
//
RET Ray_Sphere(Buff In)
{
	RET ret = (RET) 0;
	float3 E = In.E;
	float3 V = In.V;
	float3 P, N;

	//���C���J�������(�̋t��ԁj�ŕϊ� �i�J�����Ɓh�����h�悤�ɓ��������߁B���f���̏ꍇ�Ƃ͗��R���قȂ�j
	E = mul(float4(E, 1), g_mIV);
	V = mul(V, (float3x3) g_mIV); //�����x�N�g���Ɉړ������������Ă͂��߁i�������ς��j	

	////����ɁA���̃��f���i���j�̎p���̋t�s��ł��ϊ��@�i���f���̃��[�J���n�Ƀ��C�𓖂ĂĂ��邽�߁j
	E = mul(float4(E, 1), g_mSphereIW);
	V = mul(V, (float3x3) g_mSphereIW); //�����x�N�g���Ɉړ������������Ă͂��߁i�������ς��j
	
	V = normalize(V);

	float t = 0; //���C�̌�_�܂ł̒���
	float a = 0, b = 0, c = 0; //2�����̌W��
	float D = 0; //���ʎ�

	a = dot(V, V);
	b = 2 * dot(E, V);
	c = dot(E, E) - g_Radius * g_Radius;

	D = b * b - 4 * a * c;

	if (D > 0)//��_�͂Q�i�����A�g��1�̌�_�͊m��ł���j
	{
		t = (-b - sqrt(D)) / 2 * a;
	}
	else if (D == 0)//��_�͂ЂƂ���
	{
		t = (-b - sqrt(D)) / 2 * a;
	}
	else //��_�Ȃ�
	{
		t = -1;
		return ret;
	}

	P = E + t * V; //��_�́A���C�̎����̂܂�
	N = P - float3(0, 0, 0); //�@���͋��̒��S�����_�Ɍ������x�N�g�� �@�����������C�̂ق����Ȃ��Ă���̂�Sphere.pos���g���Ă͂܂���  ���̃��[�J���n�̒��S���g��
	N = normalize(N);
	
	ret.P = P;
	ret.N = N;
	ret.d = t;

	return ret;
}
//
//
//�����o�[�g�V�F�[�f�B���O���v�Z����֐�
float3 Lambert_Shade(float3 L, float3 N, float3 Diffuse)
{
	float LN = dot(L, N);
	LN = max(0, LN);
	float r = Diffuse.x * LN;
	float g = Diffuse.y * LN;
	float b = Diffuse.z * LN;

	return float3(r, g, b);
}
[numthreads(1, 1, 1)]
//
//
//�X���b�h���̓X�N���[���s�N�Z�����ŕς��l�Ȃ̂ŁA�V�F�[�_�[���ŃX���b�h�����w��ł��Ȃ��B�Ȃ̂ŃX���b�h��(1,1,1)�@�A�v�����iDispatch)�ŕ����񐔂��w��B
void main(uint3 id : SV_DispatchThreadID, uint3 gid : SV_GroupID)
{
	uint index = id.y * 1280 + id.x;

	float3 C = float3(80, 80, 80); //�ŏI�F

	RET ret = (RET) 0;
	ret = Ray_Sphere(BufferInput[index]);
	if (ret.d > 0)
	{
		float3 L = g_LightPos - ret.P;
		L = normalize(L);
		C = Lambert_Shade(L, ret.N, float3(3, 125, 251));
	}

	BufferOut[index].color = ((int) C.z) << 16 | ((int) C.y) << 8 | ((int) C.x);
}