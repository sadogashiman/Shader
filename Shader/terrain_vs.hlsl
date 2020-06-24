cbuffer Matrixbuffer
{
    matrix world;
    matrix view;
    matrix projection;
};

struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
    //計算のために代入
    input.position.w = 1.0F;
    
    //頂点座標を計算
    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);
    
    //テクスチャUVを保存
    output.tex = input.tex;
    
    //法線にワールド行列のみ計算をかける
    output.normal = mul(input.normal, (float3x3) world);
    
    //法線を正規化
    output.normal = normalize(output.normal);
    
    //出力
    return output;
}