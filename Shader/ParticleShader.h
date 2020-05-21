#pragma once
#include"Support.h"
class ParticleShader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};
	ComPtr<ID3D11VertexShader> vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout> layout_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	ComPtr<ID3D11SamplerState> samplerstate_;
	std::unique_ptr<Support> support_;

	void renderrShader(const int IndexCount);
	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

public:
	ParticleShader();
	~ParticleShader();
	bool init();
	bool render(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);
	void destroy();
};

