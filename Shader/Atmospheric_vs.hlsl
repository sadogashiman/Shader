#include"Atmosphericmath.hlsli"
cbuffer MatrixBuffer : register(b0)
{
    column_major Matrix world;
    column_major Matrix view;
    column_major Matrix projection;
};


PixelInputType main(float4 position : POSITION,
    uniform float3 camerapos, //�J�������W
    uniform float3 lightdir, //�����ւ̃x�N�g��
    uniform float3 invwavelength, //RGB�̂P/pow(wavelength,4)
    uniform float cameraheight, //�J�����̌��݂̍���
    uniform float cameraheight2, //cameraheight^2
    uniform float outerradius, //��C�̔��a
    uniform float outerradius2, //outerradius^2
    uniform float innerradius, //�f���̔��a
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