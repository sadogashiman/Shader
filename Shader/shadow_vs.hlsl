
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
};

cbuffer LightBuffer
{
    float3 lightPosition;
    float padding;
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
    float3 lightPos : TEXCOORD2;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    float4 worldPosition;

    input.position.w = 1.0f;
    
    //頂点座標を計算
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    //カメラから見た座標を計算
    output.lightViewPosition = mul(input.position, worldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, lightProjectionMatrix);

    //テクスチャを保存
    output.tex = input.tex;

    //ワールド行列にのみ法線を適応
    output.normal = mul(input.normal, (float3x3)worldMatrix);

    //法線を正規化
    output.normal = normalize(output.normal);

    //ワールド行列と座標を計算
    worldPosition = mul(input.position, worldMatrix);

    //ライト座標を計算
    output.lightPos = lightPosition.xyz - worldPosition.xyz;

    //ライト座標を正規化
    output.lightPos = normalize(output.lightPos);

    return output;
}