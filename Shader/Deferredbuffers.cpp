#include "stdafx.h"
#include "Deferredbuffers.h"
#include"Direct3D.h"
#include"release.h"

Deferredbuffers::Deferredbuffers()
{
	ZeroMemory(this, sizeof(Deferredbuffers));
}

Deferredbuffers::~Deferredbuffers()
{
}

bool Deferredbuffers::init(const int TextureWidth, const int TextureHeight, const float ScreenDepth, const float ScreenNear)
{
	D3D11_TEXTURE2D_DESC texturedesc;
	HRESULT hr;
	D3D11_RENDER_TARGET_VIEW_DESC rendertargetviewdesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderresouceviewdesc;
	D3D11_TEXTURE2D_DESC depthbufferdesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthstencilviewdesc;


	//�����_�[�^�[�Q�b�g�e�N�X�`����������
	ZeroMemory(&texturedesc, sizeof(&texturedesc));

	//�����_�[�^�[�Q�b�g�e�N�X�`���̐ݒ�
	texturedesc.Width = TextureWidth;
	texturedesc.Height = TextureHeight;
	texturedesc.MipLevels = 1;
	texturedesc.ArraySize = 1;
	texturedesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texturedesc.SampleDesc.Count = 1;
	texturedesc.SampleDesc.Quality = 0;
	texturedesc.Usage = D3D11_USAGE_DEFAULT;
	texturedesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texturedesc.CPUAccessFlags = 0;
	texturedesc.MiscFlags = 0;

	//�����_�[�^�[�Q�b�g�e�N�X�`���z����쐬
	for (int i = 0; i < kBuffer_cnt; i++)
	{
		hr = Direct3D::getInstance()->getDevice()->CreateTexture2D(&texturedesc, NULL, &rendertargettexturearray_[i]);
		if (FAILED(hr))
		{
			return false;
		}
	}

	//�����_�[�^�[�Q�b�g�r���[�̐ݒ�
	rendertargetviewdesc.Format = texturedesc.Format;
	rendertargetviewdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rendertargetviewdesc.Texture2D.MipSlice = 0;

	//�����_�[�^�[�Q�b�g�r���[���쐬
	for (int i = 0; i < kBuffer_cnt; i++)
	{
		hr = Direct3D::getInstance()->getDevice()->CreateRenderTargetView(rendertargettexturearray_[i], &rendertargetviewdesc, &rendertargetviewarray_[i]);
		if (FAILED(hr))
		{
			return false;
		}
	}

	//�V�F�[�_�[���\�[�X�r���[��ݒ�
	shaderresouceviewdesc.Format = texturedesc.Format;
	shaderresouceviewdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderresouceviewdesc.Texture2D.MostDetailedMip = 0;
	shaderresouceviewdesc.Texture2D.MipLevels = 1;

	//�V�F�[�_�[���\�[�X�r���[�̍쐬
	for (int i = 0; i < kBuffer_cnt; i++)
	{
		hr = Direct3D::getInstance()->getDevice()->CreateShaderResourceView(rendertargettexturearray_[i], &shaderresouceviewdesc, &shaderresourceviewarray_[i]);
		if (FAILED(hr))
		{
			return false;
		}
	}

	//�[�x�o�b�t�@��������
	ZeroMemory(&depthbufferdesc, sizeof(depthbufferdesc));

	//�[�x�o�b�t�@��ݒ�
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

	//�ݒ肵�����e�Ő[�x�o�b�t�@�̃e�N�X�`�����쐬
	hr = Direct3D::getInstance()->getDevice()->CreateTexture2D(&depthbufferdesc, NULL, &depthstencilbuffer_);
	if (FAILED(hr))
	{
		return false;
	}

	//�X�e���V���r���[�̐�����������
	ZeroMemory(&depthstencilviewdesc, sizeof(depthstencilviewdesc));

	//�X�e���V���r���[��ݒ�
	depthstencilviewdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthstencilviewdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthstencilviewdesc.Texture2D.MipSlice = 0;

	//�X�e���V���r���[���쐬
	hr = Direct3D::getInstance()->getDevice()->CreateDepthStencilView(depthstencilbuffer_, &depthstencilviewdesc, &depthstencilview_);
	if (FAILED(hr))
	{
		return false;
	}

	//�r���[�|�[�g�������_�����O�p�ɃZ�b�g�A�b�v
	viewport_.Width = static_cast<float>(TextureWidth);
	viewport_.Height = static_cast<float>(TextureHeight);
	viewport_.MinDepth = 0.0F;
	viewport_.MaxDepth = 1.0F;
	viewport_.TopLeftX = 0.0F;
	viewport_.TopLeftY = 0.0F;

	return true;
}

void Deferredbuffers::destroy()
{
	SAFE_RELEASE(depthstencilview_);
	SAFE_RELEASE(depthstencilbuffer_);
	for (int i = 0; i < kBuffer_cnt; i++)
	{
		SAFE_RELEASE(shaderresourceviewarray_[i]);
		SAFE_RELEASE(rendertargetviewarray_[i]);
		SAFE_RELEASE(rendertargettexturearray_[i]);
	}
}

void Deferredbuffers::setRenderTargets()
{
	Direct3D::getInstance()->getContext()->OMSetRenderTargets(kBuffer_cnt, rendertargetviewarray_, depthstencilview_);

	//�r���[�|�[�g��ݒ�
	Direct3D::getInstance()->getContext()->RSSetViewports(1, &viewport_);

}

void Deferredbuffers::clearRenderTargets(XMVECTORF32 Color)
{
	//�����_�[�^�[�Q�b�g�o�b�t�@���N���A
	for (int i = 0; i < kBuffer_cnt; i++)
	{
		Direct3D::getInstance()->getContext()->ClearRenderTargetView(rendertargetviewarray_[i], Color);
	}

	//�[�x�o�b�t�@���N���A
	Direct3D::getInstance()->getContext()->ClearDepthStencilView(depthstencilview_, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0F, 0);
}

ID3D11ShaderResourceView* Deferredbuffers::getShaderResourceView(int TextureNumber)
{
	return shaderresourceviewarray_[TextureNumber];
}
