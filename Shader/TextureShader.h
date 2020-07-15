#pragma once
#include"Support.h"
#include "Direct3D.h"

class Textureshader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};
	Direct3D* instanceptr_;
	ComPtr<ID3D11VertexShader> vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout> layout_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	ComPtr<ID3D11SamplerState> samplerstate_;
	std::unique_ptr<Support> support_;

	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);
	void renderShader(const int Indexcount);
public:
	Textureshader();
	~Textureshader();
	bool init();
	bool render(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);
};

