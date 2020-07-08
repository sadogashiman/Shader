
cbuffer MatrixBuffer
{
    matrix world;
    matrix view;
    matrix projection;
    matrix lightView;
    matrix lightProjection;
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
    float4 lightViewPosition : TEXCOORD1;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    input.position.w = 1.0f;
    
    //頂点座標を計算
    output.position = mul(input.position, world);
    output.position = mul(output.position, view);
    output.position = mul(output.position, projection);

    //カメラから見た座標を計算
    output.lightViewPosition = mul(input.position, world);
    output.lightViewPosition = mul(output.lightViewPosition, lightView);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjection);

    //テクスチャを保存
    output.tex = input.tex;

    //ワールド行列にのみ法線を適応
    output.normal = mul(input.normal, (float3x3)world);

    //法線を正規化
    output.normal = normalize(output.normal);

    return output;
}