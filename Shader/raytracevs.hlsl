struct PixelInputType
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType main( float4 Pos : POSITION ,float2 Tex:TEXCOORD)
{
    PixelInputType output;
    
    //�s�N�Z���V�F�[�_�[�ɂ��̂܂܏o��
    output.pos = Pos;
    output.tex = Tex;
    
    return output;
}