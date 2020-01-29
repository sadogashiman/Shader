#pragma once
#include"Support.h"

class Deferredshader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};
	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);
	void renderShader(const int Indexcount);

	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	ID3D11InputLayout* layout_;
	ID3D11SamplerState* samplestatewrap_;
	ID3D11Buffer* matrixbuffer_;
	std::unique_ptr<Support> support_;


public:
	Deferredshader();
	~Deferredshader();
	bool init();
	void destroy();
	bool render(const int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture);

};

