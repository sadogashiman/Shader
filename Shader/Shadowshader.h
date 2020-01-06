#pragma once
#include"release.h"
#include"checkinputlayout.h"
class Shadowshader
{
private:
    struct MatrixBufferType
    {
        Matrix world;
        Matrix view;
        Matrix projection;
        Matrix lightview;
        Matrix lightprojection;
        Matrix lightview2;
        Matrix lightprojection2;
    };

    struct LightBufferType
    {
        Vector4 AmbientColor;
        Vector4 DiffuseColor;
        Vector4 DiffuseColor2;
    };

    struct LightBufferType2
    {
        Vector3 lightposition;
        float padding1;
        Vector3 lightposition2;
        float padding2;
    };

    bool initShader(std::filesystem::path vsFileName, std::filesystem::path psFileName);
    void destroyShader();
    bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, Matrix lightview, Matrix Lightprojection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 lightposition,Vector4 Ambientcolor,Vector4 Diffusecolor,Matrix Lightview2,Matrix LightProjection2,ID3D11ShaderResourceView* Depthmaptexture2,Vector3 Lightposition2,Vector4 Diffusecolor2);
    void renderShader(const int Indexcount);
    ID3D11VertexShader* vertexshader_;
    ID3D11PixelShader* pixelshader_;
    ID3D11InputLayout* layout_;
    ID3D11SamplerState* samplestateclamp_;
    ID3D11SamplerState* samplestatewrap_;
    ID3D11Buffer* matrixbuffer_;
    ID3D11Buffer* lightbuffer_;
    ID3D11Buffer* lightbuffer2_;

public:
    Shadowshader();
    ~Shadowshader();
    bool init();
    void destroy();
    bool render(const int Indexcound, const Matrix World, const Matrix View, const Matrix Projection,const Matrix lightview,const Matrix lightprojection, ID3D11ShaderResourceView* texture,ID3D11ShaderResourceView* Depthmaptexture,Vector3 lightposition, Vector4 Ambientcolor,Vector4 Diffusecolor,Matrix Lightview2,Matrix Lightprojection2,ID3D11ShaderResourceView* Depthmap,Vector3 Lightposition2,Vector4 Diffusecolor2);
};

