#pragma once
#include"release.h"
#include"error.h"
#include"checkinputlayout.h"
class Depthshader
{
private:
    struct MatrixBufferType
    {
        Matrix world;
        Matrix view;
        Matrix projection;
    };
    bool initShader(std::filesystem::path vsFilename, std::filesystem::path psFilename);
    void destroyShader();
    bool setShaderParameters(Matrix worldMatrix, Matrix viewMatrix, Matrix projectionMatrix);
    void renderShader(int indexCount);

    ID3D11VertexShader* vertexshader_;
    ID3D11PixelShader* pixelshader_;
    ID3D11InputLayout* layout_;
    ID3D11Buffer* matrixbuffer_;

public:
    Depthshader();
    ~Depthshader();
    bool init();
    void destroy();
    bool render(int indexCount, Matrix worldMatrix, Matrix viewMatrix, Matrix projectionMatrix);
};

