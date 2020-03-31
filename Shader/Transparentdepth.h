#pragma once
#include"Support.h"

class Transparentdepth
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix Projection;
	};

	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);
	bool renderShader(const int IndexCount);

	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	ID3D11InputLayout* layout_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	ComPtr<ID3D11SamplerState> samplerstate_;
	std::unique_ptr<Support> support_;

public:
	Transparentdepth();
	~Transparentdepth();
	bool init();
	bool render(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);
	void destroy();

};
