cbuffer MatrixBuffer
{
    matrix world;
    matrix view;
    matrix projection;
};

struct VertexInputType
{
    float4 position:POSITION;
    float2 tex : TEXCOORD;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

PixelInputType main( VertexInputType input )
{
    
    PixelInputType output;
    
    //計算のために1を代入
    input.position.w = 1.0F;
    
    //座標を計算
    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    
    //テクスチャを保存
    output.tex = input.tex;
    
	return output;
}