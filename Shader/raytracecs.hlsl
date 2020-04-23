struct Buff
{
    float3 startpoint;    //���C�̎��_(����)
    float3 direction;     //���C�̕���(����)
    float3 intersection;  //��_
    float3 normal;        //��_�̖@��
    float distance;       //���_�����_�܂ł̋���
    uint color;           //�s�N�Z���J���[
};

cbuffer global : register(b0)
{
    float3 position : packoffset(c0); //���S���W
    float radius : packoffset(c1);    //���a
    matrix modeliw : packoffset(c2);  //���f���̍s��
    matrix cameraiw : packoffset(c6); //�J�����̍s��
    float3 lightposition : packoffset(c10); //���C�g�̍��W
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
    
    //���C���J������Ԃŕϊ�
    float4 e = float4(start.x, start.y, start.z, 1.0F);
    
    e = mul(e, cameraiw);
    dir = mul(dir, (float3x3) cameraiw);
    
    e = mul(e, modeliw);
    start = e.xyz;
    dir = mul(dir, (float) modeliw);
    
    dir = normalize(dir);
    
    float t = 0; //���C�̌�_�܂ł̒���
    float a = 0, b = 0, c = 0; //2�����̌W��
    float d = 0; //���ʎ�
    
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
    
    //��_�̓��C�̎������̂܂�
    inter = start + t * dir;
    normal = inter - float3(0.0F, 0.0F, 0.0F);
    normal = normalize(normal); //���K��
    
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


//�X���b�h���̓V�F�[�_�[���Ŏw��ł��Ȃ��̂�1,1,1�ɂ��Ă���
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID,uint3 gid:SV_GroupID )
{
    uint index = DTid.y * 484 + DTid.x;
    
    //�f�t�H���g�̏o�͐F��ݒ�
    float3 finalcolor = float3(80.0F, 80.0F,80.0F);
    
    Buff ret;
    ret = (Buff) 0; //������
    
    ret = ray_Sphere(BufferInput[index]);
    
    if(ret.distance>0)
    {
        float3 L = lightposition - ret.intersection;
        L = normalize(L);
        finalcolor = lambert_Shade(L, ret.normal, float3(3.0F, 125.0F, 251.0F));
    }
    
    BufferOutput[index].color = ((int) finalcolor.z) << 16 | ((int) finalcolor.y) << 8 | ((int) finalcolor.x);
}