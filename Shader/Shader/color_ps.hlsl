struct PixelInputType
{
    float4 position : SV_POSITION;
};

cbuffer ColorBuffer:register(b0)
{
    float4 color;
};


float4 main() : SV_TARGET
{
    //入力された色を出力
    return color;
}