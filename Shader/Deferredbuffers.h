#pragma once
#include"Direct3D.h"

const int kBuffer_cnt = 2;
class Deferredbuffers
{
private:
	Direct3D* instanceptr_;
	ID3D11Texture2D* rendertargettexturearray_[kBuffer_cnt];
	ID3D11RenderTargetView* rendertargetviewarray_[kBuffer_cnt];
	ID3D11ShaderResourceView* shaderresourceviewarray_[kBuffer_cnt];
	ID3D11Texture2D* depthstencilbuffer_;
	ID3D11DepthStencilView* depthstencilview_;
	D3D11_VIEWPORT viewport_;
public:
	Deferredbuffers();
	~Deferredbuffers();

	bool init(const int TextureWidth,const int TextureHeight, const float ScreenDepth, const float ScreenNear);
	void destroy();
	void setRenderTargets();
	void clearRenderTargets(XMVECTORF32 Color);
	ID3D11ShaderResourceView* getShaderResourceView(int TextureNumber);

};

