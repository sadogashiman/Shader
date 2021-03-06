#pragma once
class Rendertexture
{
private:
	ComPtr<ID3D11Texture2D> rendertargettexture_;
	ComPtr<ID3D11RenderTargetView> rendertargetview_;
	ComPtr<ID3D11ShaderResourceView> shaderresouceview_;
	ComPtr<ID3D11Texture2D> depthstencilbuffer_;
	ComPtr<ID3D11DepthStencilView> depthstencilview_;
	D3D11_VIEWPORT viewport_;
	Matrix projection_;
	Matrix ortho_;

public:
	Rendertexture();
	~Rendertexture();
	bool init(int TextureWidth, int TextureHeight);
	void setRenderTarget();
	void clearRenderTarget(XMVECTORF32 Color = Colors::Black);

	//get
	inline Matrix getProjectionMatrix()const { return projection_; }
	inline Matrix getOrthoMatrix()const { return ortho_; }
	inline ID3D11ShaderResourceView* getShaderResouceView()const { return shaderresouceview_.Get(); }


};

