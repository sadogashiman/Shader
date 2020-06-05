#include "stdafx.h"
#include "Rendertexture.h"
#include "Direct3D.h"

Rendertexture::Rendertexture()
{
    ZeroMemory(this, sizeof(Rendertexture));
}

Rendertexture::~Rendertexture()
{
}

bool Rendertexture::init(int TextureWidth, int TextureHeight)
{
    D3D11_TEXTURE2D_DESC texturedesc;
    HRESULT hr;
    D3D11_RENDER_TARGET_VIEW_DESC rendertargetviewdesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderresouceviewdesc;
    D3D11_TEXTURE2D_DESC depthbufferdesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthstencilviewdesc;

    //レンダーターゲットテクスチャの説明
    ZeroMemory(&texturedesc, sizeof(texturedesc));

    //レンダーターゲットテクスチャの設定
    texturedesc.Width = TextureWidth;
    texturedesc.Height = TextureHeight;
    texturedesc.MipLevels = 1;
    texturedesc.ArraySize = 1;
    texturedesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    texturedesc.SampleDesc.Count = 1;
    texturedesc.Usage = D3D11_USAGE_DEFAULT;
    texturedesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texturedesc.CPUAccessFlags = 0;
    texturedesc.MiscFlags = 0;

    //レンダーターゲットテクスチャを作成
    hr = Direct3D::getInstance()->getDevice()->CreateTexture2D(&texturedesc, NULL, &rendertargettexture_);
    if (FAILED(hr))
    {
        return false;
    }

    //レンダーターゲットビューの設定
    rendertargetviewdesc.Format = texturedesc.Format;
    rendertargetviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rendertargetviewdesc.Texture2D.MipSlice = 0;

    //レンダーターゲットビューの作成
    hr = Direct3D::getInstance()->getDevice()->CreateRenderTargetView(rendertargettexture_, &rendertargetviewdesc, &rendertargetview_);
    if (FAILED(hr))
    {
        return false;
    }

    //シェーダーリソースの設定
    shaderresouceviewdesc.Format = texturedesc.Format;
    shaderresouceviewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderresouceviewdesc.Texture2D.MostDetailedMip = 0;
    shaderresouceviewdesc.Texture2D.MipLevels = 1;

    //シェーダーリソースビューの作成
    hr = Direct3D::getInstance()->getDevice()->CreateShaderResourceView(rendertargettexture_, &shaderresouceviewdesc, &shaderresouceview_);
    if (FAILED(hr))
    {
        return false;
    }

    ZeroMemory(&depthbufferdesc, sizeof(depthbufferdesc));

    depthbufferdesc.Width = TextureWidth;
    depthbufferdesc.Height = TextureHeight;
    depthbufferdesc.MipLevels = 1;
    depthbufferdesc.ArraySize = 1;
    depthbufferdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthbufferdesc.SampleDesc.Count = 1;
    depthbufferdesc.SampleDesc.Quality = 0;
    depthbufferdesc.Usage = D3D11_USAGE_DEFAULT;
    depthbufferdesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthbufferdesc.CPUAccessFlags = 0;
    depthbufferdesc.MiscFlags = 0;

    hr = Direct3D::getInstance()->getDevice()->CreateTexture2D(&depthbufferdesc, NULL, &depthstencilbuffer_);
    if (FAILED(hr))
    {
        return false;
    }

    ZeroMemory(&depthstencilviewdesc, sizeof(depthstencilviewdesc));

    depthstencilviewdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthstencilviewdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthstencilviewdesc.Texture2D.MipSlice = 0;
    hr = Direct3D::getInstance()->getDevice()->CreateDepthStencilView(depthstencilbuffer_, &depthstencilviewdesc, &depthstencilview_);
    if (FAILED(hr))
    {
        return false;
    }

    viewport_.Width = static_cast<float>(TextureWidth);
    viewport_.Height = static_cast<float>(TextureHeight);
    viewport_.MinDepth = 0.0F;
    viewport_.MaxDepth = 1.0F;
    viewport_.TopLeftX = 0.0F;
    viewport_.TopLeftY = 0.0F;

    projection_ = XMMatrixPerspectiveFovLH((static_cast<float>(XM_PI) / 4.0F), (static_cast<float>(TextureWidth) / static_cast<float>(TextureHeight)), static_cast<float>(kScreen_near), static_cast<float>(kScreen_depth));

    ortho_ = XMMatrixOrthographicLH(static_cast<float>(TextureWidth) , static_cast<float>(TextureHeight),kScreen_near,kScreen_depth);

    return true;
}

void Rendertexture::destroy()
{
    SAFE_RELEASE(depthstencilview_);
    SAFE_RELEASE(depthstencilbuffer_);
    SAFE_RELEASE(shaderresouceview_);
    SAFE_RELEASE(rendertargetview_);
    SAFE_RELEASE(rendertargettexture_);
}

void Rendertexture::setRenderTarget()
{
    //レンダーターゲットビューと深度ステンシルバッファを出力レンダーパイプラインにバインド
    Direct3D::getInstance()->getContext()->OMSetRenderTargets(1, &rendertargetview_, depthstencilview_);

    Direct3D::getInstance()->getContext()->RSSetViewports(1, &viewport_);
}

void Rendertexture::clearRenderTarget(XMVECTORF32 Color)
{
    //バックバッファクリア
    Direct3D::getInstance()->getContext()->ClearRenderTargetView(rendertargetview_, Color);

    //深度バッファクリア
    Direct3D::getInstance()->getContext()->ClearDepthStencilView(depthstencilview_,D3D11_CLEAR_DEPTH,1.0F,0);
}
