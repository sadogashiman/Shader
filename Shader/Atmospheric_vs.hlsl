cbuffer MatrixBuffer : register(b0)
{
    column_major Matrix world;
    column_major Matrix view;
    column_major Matrix projection;
};


struct VertexInputType
{
    float4 position : POSITION;
    float3 camerapos:TEXCOORD1;
    float3 lightdir:TEXCOORD2;
    
    
};

struct PixelInputType
{
    //float4 pos:SV_Position;
};



PixelInputType main( VertexInputType Input )
{
    PixelInputType output;
    Input.position.w = 1.0F;
    
   
    return output;
}