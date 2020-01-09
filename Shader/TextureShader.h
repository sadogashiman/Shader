#pragma once
#include"Support.h"

class Textureshader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	ID3D11InputLayout* layout_;
	ID3D11Buffer* matrixbuffer_;
	ID3D11SamplerState* samplerstate_;

	bool initShader(const wchar_t* vsFileName, const wchar_t* psFileName);
	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);
	void renderShader(const int Indexcount);
	void destroyShader();
public:
	bool init();
	bool render(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);
	void destroy();
};

