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
    //“ü—Í‚³‚ê‚½F‚ğo—Í
    return color;
}