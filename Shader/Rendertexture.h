#pragma once
#include"release.h"
class Rendertexture
{
private:
    ID3D11Texture2D* rendertargettexture_;
    ID3D11RenderTargetView* rendertargetview_;
    ID3D11ShaderResourceView* shaderresouceview_;
    ID3D11Texture2D* depthstencilbuffer_;
    ID3D11DepthStencilView* depthstencilview_;
    D3D11_VIEWPORT viewport_;
    Matrix projection_;
    Matrix ortho_;

public:
    Rendertexture();
    ~Rendertexture();
    bool init(ID3D11Device* Device, int TextureWidth, int TextureHeight,float Screendepth,float ScreenNear);
    void destroy();
    void setRenderTarget(ID3D11DeviceContext* Devicecontext);
    void clearRenderTarget(ID3D11DeviceContext* Devicecontext,float Red, float Blue, float Green, float Alpha);
    ID3D11ShaderResourceView* getShaderResouceView();

    //get
    void getProjection(Matrix& Projection);
    void getOrtho(Matrix& Ortho);

};

