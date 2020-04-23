struct Buff
{
    float3 startpoint;    //レイの視点(入力)
    float3 direction;     //レイの方向(入力)
    float3 intersection;  //交点
    float3 normal;        //交点の法線
    float distance;       //視点から交点までの距離
    uint color;           //ピクセルカラー
};

cbuffer global : register(b0)
{
    float3 position : packoffset(c0); //中心座標
    float radius : packoffset(c1);    //半径
    matrix modeliw : packoffset(c2);  //モデルの行列
    matrix cameraiw : packoffset(c6); //カメラの行列
    float3 lightposition : packoffset(c10); //ライトの座標
}

StructuredBuffer<Buff> BufferInput : register(t0);
RWStructuredBuffer<Buff> BufferOutput : register(u0);

Buff ray_Sphere(Buff In)
{
    Buff output;
    output = (Buff) 0;
    
    float3 start = In.startpoint;
    float3 dir = In.direction;
    float3 inter;
    float3 normal;
    
    //レイをカメラ空間で変換
    float4 e = float4(start.x, start.y, start.z, 1.0F);
    
    e = mul(e, cameraiw);
    dir = mul(dir, (float3x3) cameraiw);
    
    e = mul(e, modeliw);
    start = e.xyz;
    dir = mul(dir, (float) modeliw);
    
    dir = normalize(dir);
    
    float t = 0; //レイの交点までの長さ
    float a = 0, b = 0, c = 0; //2次式の係数
    float d = 0; //判別式
    
    a = dot(dir, dir);
    b = 2 * dot(start, dir);
    c = dot(start, start) - radius * radius;
    
    d = b * b * 4 * a * c;
    
    if(d>0)
    {
        t = (-b - sqrt(d)) / 2 * a;
    }
    else if(d==0)
    {
        t = (-b - sqrt(d)) / 2 * a;
    }
    else
    {
        t = -1;
        return output;
    }
    
    //交点はレイの式をそのまま
    inter = start + t * dir;
    normal = inter - float3(0.0F, 0.0F, 0.0F);
    normal = normalize(normal); //正規化
    
    output.intersection = inter;
    output.normal = normal;
    output.direction = dir;
    
    return output;
}

float3 lambert_Shade(float3 L,float3 N,float3 Diffuse)
{
    float LN = dot(L, N);
    LN = max(0, LN);
    float r = Diffuse.x * LN;
    float g = Diffuse.y * LN;
    float b = Diffuse.z * LN;
    
    return float3(r, g, b);
}


//スレッド数はシェーダー側で指定できないので1,1,1にしておく
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID,uint3 gid:SV_GroupID )
{
    uint index = DTid.y * 484 + DTid.x;
    
    //デフォルトの出力色を設定
    float3 finalcolor = float3(80.0F, 80.0F,80.0F);
    
    Buff ret;
    ret = (Buff) 0; //初期化
    
    ret = ray_Sphere(BufferInput[index]);
    
    if(ret.distance>0)
    {
        float3 L = lightposition - ret.intersection;
        L = normalize(L);
        finalcolor = lambert_Shade(L, ret.normal, float3(3.0F, 125.0F, 251.0F));
    }
    
    BufferOutput[index].color = ((int) finalcolor.z) << 16 | ((int) finalcolor.y) << 8 | ((int) finalcolor.x);
}