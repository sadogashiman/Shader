#pragma once
#include"Support.h"
#include"Direct3D.h"

class MultitexShader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray,const int Texturenum);
	void renderShader(const int Indexcount);

	Direct3D* instanceptr_;
	ComPtr<ID3D11VertexShader> vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout> layout_;
	ComPtr<ID3D11SamplerState> samplerstate_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	std::unique_ptr<Support> support_;

public:
	MultitexShader();
	~MultitexShader();
	bool init();
	bool render(const int Indexcount,Matrix World,Matrix View,Matrix Projection, ID3D11ShaderResourceView** Texturearray, const int Texturenum);
};

