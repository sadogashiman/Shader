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
    
    //環境光を設定
    color = ambient;
    
    //計算のためにライト方向を反転
    lightdir = -lightdirection;
    
    //光量を計算
    lightintensity = saturate(dot(input.normal, lightdir));
    
    if(lightintensity>0.0F)
    {
        //拡散色と光量に基づいて拡散色を決定
        color += (diffuse * lightintensity);
    }
    
    //色を飽和
    color = saturate(color);
    
    return color;
    
}