struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXTURE0;
};


float4 main(PixelInputType input) : SV_TARGET0
{
    float depthValue;
    float4 color;

    //�[�x�l���v�Z
    depthValue = input.depthPosition.z / input.depthPosition.w;

    color = float4(depthValue, depthValue, depthValue, 1.0f);

    return color;
}
