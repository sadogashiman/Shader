struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};


float4 main(PixelInputType Input) : SV_TARGET
{
    return float4(1.0F,1.0F,1.0F,1.0F);
}