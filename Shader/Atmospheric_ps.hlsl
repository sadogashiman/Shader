#include"Atmosphericmath.hlsli"

float4 main(
float4 Color0:COLOR0,
float4 Color1:COLOR1,
float3 Direction:TEXCOORD0,
uniform float3 LightDir,
uniform float G,
uniform float G2
) : SV_TARGET
{
    float cos = dot(LightDir, Direction) / length(Direction);
    float cos2 = cos * cos;
    
    //散乱の計算
    float4 color = getRayleighPhase(cos2) * Color0 + getMiePhase(cos, cos2, G, G2) * Color1;
    
    //アルファを設定
    color.a = color.b;
	
	return color;
}