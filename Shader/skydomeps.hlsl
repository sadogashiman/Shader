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
    
    //スカイドーム上でのこのピクセルの位置を決定
    height = Input.domposition.y;
    
    //値を正の値のみに変更
    if(height<0.0F)
    {
        height = 0.0F;
    }
    
    //値を補完してグラデーションの色を作成
    output = lerp(centorcolor, apexcolor, height);
    
    return output;
    
}