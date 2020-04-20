struct PixelInputType
{
    float4 pos : POSITION;
    float2 tex : TEXCOORD0;
};

PixelInputType main( float4 Pos : POSITION ,float2 Tex:TEXCOORD)
{
    PixelInputType output;
    
    //ピクセルシェーダーにそのまま出力
    output.pos = Pos;
    output.tex = Tex;
    
    return output;
}