cbuffer GradientBuffer
{
    float4 apexcolor;
    float4 centorcolor;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 domposition : TEXCOORD0;
};


float4 main(PixelInputType Input) : SV_TARGET
{
    float height;
    float4 output;
    
    //�X�J�C�h�[����ł̂��̃s�N�Z���̈ʒu������
    height = Input.domposition.y;
    
    //�l�𐳂̒l�݂̂ɕύX
    if(height<0.0F)
    {
        height = 0.0F;
    }
    
    //�l��⊮���ăO���f�[�V�����̐F���쐬
    output = lerp(centorcolor, apexcolor, height);
    
    return output;
    
}