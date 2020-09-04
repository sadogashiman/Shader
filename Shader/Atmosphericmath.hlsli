struct PixelInputType
{
    float4 position : POSITION;
    float4 rayleighcolor : COLOR0;
    float4 miecolor : COLOR1;
    float3 texture : TEXCOORD;
};

//�T���v�����O�|�C���g��
const int nSamples = 2;
const float fSamples = (float) nSamples;

//��C�̍���
const float fScaleDepth = 0.25F;
const float fInvScaleDepth = 1.0 / fScaleDepth;

//�X�P�[�������߂������
float Scale(float Cos)
{
    float x = 1.0 - Cos;
    return fScaleDepth * exp(-0.00287 + x * (0.459 + x * (3.83 + x * (-6.80 + x * 5.25))));
}

//Mie�U�����v�Z
float getMiePhase(float Cos,float Cos2,float G,float G2)
{
    return 1.5 * ((1.0 - G2) / (2.0 + G2)) * (1.0 + Cos2) / pow(1.0 + G2 - 2.0 * G * Cos, 1.5);
}

//Rayleigh�U�����v�Z
float getRayleighPhase(float Cos2)
{
    return 0.75 + 0.75 * Cos2;
}

//���Ƌ��̋߂���_��Ԃ�
float getNearIntersection(float3 Position,float3 Ray,float Distance2,float Radius2)
{
    float b = 2.0 * dot(Position, Ray);
    float c = Distance2 - Radius2;
    float det = max(0.0, b * b - 4.0 * c);
    return 0.5 * (-b - sqrt(det));
}

//���Ƌ��̉�����_��Ԃ�
float getFarIntersection(float3 Position,float3 Ray,float Distance2,float Radius2)
{
    float b = 2.0 * dot(Position, Ray);
    float c = Distance2 - Radius2;
    float det = max(0.0, b * b - 4.0 * c);
    return 0.5 * (-b + sqrt(det));
}