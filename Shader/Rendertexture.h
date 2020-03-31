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
    bool init(int TextureWidth, int TextureHeight);
    void destroy();
    void setRenderTarget();
    void clearRenderTarget(XMVECTORF32 Color = Colors::Black);

    //get
    inline Matrix getProjection()const { return projection_; }
    inline Matrix getOrtho()const { return ortho_; }
    inline ID3D11ShaderResourceView* getShaderResouceView()const { return shaderresouceview_; }


};

