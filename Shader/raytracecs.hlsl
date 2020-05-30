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
	float3 g_Pos : packoffset(c0); //中心座標
	float g_Radius : packoffset(c1); //半径
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

	//レイをカメラ空間(の逆空間）で変換 （カメラと”同じ”ように動かすため。モデルの場合とは理由が異なる）
	E = mul(float4(E, 1), g_mIV);
	V = mul(V, (float3x3) g_mIV); //方向ベクトルに移動成分をかけてはだめ（方向が変わる）	

	////さらに、このモデル（球）の姿勢の逆行列でも変換　（モデルのローカル系にレイを当てているため）
	E = mul(float4(E, 1), g_mSphereIW);
	V = mul(V, (float3x3) g_mSphereIW); //方向ベクトルに移動成分をかけてはだめ（方向が変わる）
	
	V = normalize(V);

	float t = 0; //レイの交点までの長さ
	float a = 0, b = 0, c = 0; //2次式の係数
	float D = 0; //判別式

	a = dot(V, V);
	b = 2 * dot(E, V);
	c = dot(E, E) - g_Radius * g_Radius;

	D = b * b - 4 * a * c;

	if (D > 0)//交点は２つ（だが、使う1個の交点は確定できる）
	{
		t = (-b - sqrt(D)) / 2 * a;
	}
	else if (D == 0)//交点はひとつだけ
	{
		t = (-b - sqrt(D)) / 2 * a;
	}
	else //交点なし
	{
		t = -1;
		return ret;
	}

	P = E + t * V; //交点は、レイの式そのまま
	N = P - float3(0, 0, 0); //法線は球の中心から交点に向かうベクトル 　せっかくレイのほうを曲げているのにSphere.posを使ってはまずい  球のローカル系の中心を使う
	N = normalize(N);
	
	ret.P = P;
	ret.N = N;
	ret.d = t;

	return ret;
}
//
//
//ランバートシェーディングを計算する関数
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
//スレッド数はスクリーンピクセル数で変わる値なので、シェーダー内でスレッド数を指定できない。なのでスレッドは(1,1,1)　アプリ側（Dispatch)で複数回数を指定。
void main(uint3 id : SV_DispatchThreadID, uint3 gid : SV_GroupID)
{
	uint index = id.y * 1280 + id.x;

	float3 C = float3(80, 80, 80); //最終色

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