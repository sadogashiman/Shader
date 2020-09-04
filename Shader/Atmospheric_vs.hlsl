#include"Atmosphericmath.hlsli"
cbuffer MatrixBuffer : register(b0)
{
    column_major Matrix world;
    column_major Matrix view;
    column_major Matrix projection;
};


PixelInputType main(float4 position : POSITION,
    uniform float3 camerapos, //カメラ座標
    uniform float3 lightdir, //光源へのベクトル
    uniform float3 invwavelength, //RGBの１/pow(wavelength,4)
    uniform float cameraheight, //カメラの現在の高さ
    uniform float cameraheight2, //cameraheight^2
    uniform float outerradius, //大気の半径
    uniform float outerradius2, //outerradius^2
    uniform float innerradius, //惑星の半径
    uniform float innerradius2, //innerradius^2
    uniform float KrEsun, //Kr*Esun
    uniform float KmEsun, //Km*Esun
    uniform float Kr4pi, //kr*4*pi
    uniform float km4pi, //km*4*pi
    uniform float scale, //1/(outerradius-innerradius)
    uniform float scaleoverscaledepth //scale/scaledepth
)
{
    PixelInputType output;
    
   
    return output;
}