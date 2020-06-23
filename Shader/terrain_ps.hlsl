cbuffer LightBuffer
{
    float4 ambient;
    float4 diffuse;
    float3 lightdirection;
    float padding;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 normal : NORMA;
};

float4 main(PixelInputType input):SV_TARGET
{
    float3 lightdir;
    float lightintensity;
    float4 color;
    
    //ŠÂ‹«Œõ‚ğİ’è
    color = ambient;
    
    //ŒvZ‚Ì‚½‚ß‚Éƒ‰ƒCƒg•ûŒü‚ğ”½“]
    lightdir = -lightdirection;
    
    //Œõ—Ê‚ğŒvZ
    lightintensity = saturate(dot(input.normal, lightdir));
    
    if(lightintensity>0.0F)
    {
        //ŠgUF‚ÆŒõ—Ê‚ÉŠî‚Ã‚¢‚ÄŠgUF‚ğŒˆ’è
        color += (diffuse * lightintensity);
    }
    
    //F‚ğ–O˜a
    color = saturate(color);
    
    return color;
    
}