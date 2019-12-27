#include "stdafx.h"
#include "Rendertexture.h"

Rendertexture::Rendertexture()
{
    rendertargettexture_ = nullptr;
    rendertargetview_ = nullptr;
    shaderresouceview_ = nullptr;
}

Rendertexture::~Rendertexture()
{
}

bool Rendertexture::init(ID3D11Device * Device, int TextureWidth, int TextureHeight, float Screendepth, float ScreenNear)
{
    D3D11_TEXTURE2D_DESC texturedesc;
    HRESULT hr;
    D3D11_RENDER_TARGET_VIEW_DESC rendertargetviewdesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderresouceviewdesc;
    D3D11_TEXTURE2D_DESC depthbufferdesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthstencilviewdesc;

    //�����_�[�^�[�Q�b�g�e�N�X�`���̐���
    ZeroMemory(&texturedesc, sizeof(texturedesc));

    //�����_�[�^�[�Q�b�g�e�N�X�`���̐ݒ�
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

    //�����_�[�^�[�Q�b�g�e�N�X�`�����쐬
    hr = Device->CreateTexture2D(&texturedesc, NULL, &rendertargettexture_);
    if (FAILED(hr))
    {
        return false;
    }

    //�����_�[�^�[�Q�b�g�r���[�̐ݒ�
    rendertargetviewdesc.Format = texturedesc.Format;
    rendertargetviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rendertargetviewdesc.Texture2D.MipSlice = 0;

    //�����_�[�^�[�Q�b�g�r���[�̍쐬
    hr = Device->CreateRenderTargetView(rendertargettexture_, &rendertargetviewdesc, &rendertargetview_);
    if (FAILED(hr))
    {
        return false;
    }

    //�V�F�[�_�[���\�[�X�̐ݒ�
    shaderresouceviewdesc.Format = texturedesc.Format;
    shaderresouceviewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    shaderresouceviewdesc.Texture2D.MostDetailedMip = 0;
    shaderresouceviewdesc.Texture2D.MipLevels = 1;

    //�V�F�[�_�[���\�[�X�r���[�̍쐬
    hr = Device->CreateShaderResourceView(rendertargettexture_, &shaderresouceviewdesc, &shaderresouceview_);
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

    hr = Device->CreateTexture2D(&depthbufferdesc, NULL, &depthstencilbuffer_);
    if (FAILED(hr))
    {
        return false;
    }

    ZeroMemory(&depthstencilviewdesc, sizeof(depthstencilviewdesc));

    depthstencilviewdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthstencilviewdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthstencilviewdesc.Texture2D.MipSlice = 0;
    hr = Device->CreateDepthStencilView(depthstencilbuffer_, &depthstencilviewdesc, &depthstencilview_);
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

    projection_ = XMMatrixPerspectiveFovLH((static_cast<float>(XM_PI) / 4.0F), (static_cast<float>(TextureWidth) / static_cast<float>(TextureHeight)), static_cast<float>(ScreenNear), static_cast<float>(Screendepth));

    ortho_ = XMMatrixOrthographicLH(static_cast<float>(TextureWidth) , static_cast<float>(TextureHeight),ScreenNear,Screendepth);

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

void Rendertexture::setRenderTarget(ID3D11DeviceContext * Devicecontext)
{

    //�����_�[�^�[�Q�b�g�r���[�Ɛ[�x�X�e���V���o�b�t�@���o�̓����_�[�p�C�v���C���Ƀo�C���h
    Devicecontext->OMSetRenderTargets(1, &rendertargetview_, depthstencilview_);

    Devicecontext->RSSetViewports(1, &viewport_);
}

void Rendertexture::clearRenderTarget(ID3D11DeviceContext * Devicecontext,float Red, float Blue, float Green, float Alpha)
{
    float color[4];

    //�o�b�t�@���N���A����F������
    color[0] = Red;
    color[1] = Green;
    color[2] = Blue;
    color[3] = Alpha;

    //�o�b�N�o�b�t�@�N���A
    Devicecontext->ClearRenderTargetView(rendertargetview_, color);

    //�[�x�o�b�t�@�N���A
    Devicecontext->ClearDepthStencilView(depthstencilview_,D3D11_CLEAR_DEPTH,1.0F,0);
}

ID3D11ShaderResourceView * Rendertexture::getShaderResouceView()
{
    return shaderresouceview_;
}

void Rendertexture::getProjection(Matrix& Projection)
{
    Projection = projection_;
}

void Rendertexture::getOrtho(Matrix& Ortho)
{
    Ortho = ortho_;
}
