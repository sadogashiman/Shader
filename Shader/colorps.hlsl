struct PixelInputType
{
    float4 position : SV_Position;
    float4 color : COLOR;
};


float4 main(PixelInputType Input) : SV_TARGET
{
    return Input.color;
}