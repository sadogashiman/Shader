#pragma once
#include"Support.h"
#include"Direct3D.h"

class Maskshader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};
	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray);
	void rendershader(int Indexcount);

	Direct3D* instanceptr_;
	ComPtr<ID3D11VertexShader> vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout> layout_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	ComPtr<ID3D11SamplerState> samplestate_;
	std::unique_ptr<Support> support_;
public:
	Maskshader();
	~Maskshader();
	bool init();
	bool render(int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView**TextureArray);
};

