struct PixelInputType
{
    float4 position : POSITION;
    float4 rayleighcolor : COLOR0;
    float4 miecolor : COLOR1;
    float3 texture : TEXCOORD;
};

//サンプリングポイント数
const int nSamples = 2;
const float fSamples = (float) nSamples;

//大気の高さ
const float fScaleDepth = 0.25F;
const float fInvScaleDepth = 1.0 / fScaleDepth;

//スケールを求める方程式
float Scale(float Cos)
{
    float x = 1.0 - Cos;
    return fScaleDepth * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

//Mie散乱を計算
float getMiePhase(float Cos,float Cos2,float G,float G2)
{
    return 1.5 * ((1.0 - G2) / (2.0 + G2)) * (1.0 + Cos2) / pow(1.0 + G2 - 2.0 * G * Cos, 1.5);
}

//Rayleigh散乱を計算
float getRayleighPhase(float Cos2)
{
    return 0.75 + 0.75 * Cos2;
}

//線と球の近い交点を返す
float getNearIntersection(float3 Position,float3 Ray,float Distance2,float Radius2)
{
    float b = 2.0 * dot(Position, Ray);
    float c = Distance2 - Radius2;
    float det = max(0.0, b * b - 4.0 * c);
    return 0.5 * (-b - sqrt(det));
}

//線と球の遠い交点を返す
float getFarIntersection(float3 Position,float3 Ray,float Distance2,float Radius2)
{
    float b = 2.0 * dot(Position, Ray);
    float c = Distance2 - Radius2;
    float det = max(0.0, b * b - 4.0 * c);
    return 0.5 * (-b + sqrt(det));
}