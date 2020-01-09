#pragma once
#include"Support.h"
class Alphamapshader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};
	void destroyshader();
	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray);
	void rendershader(int Indexcount);
	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	ID3D11InputLayout* layout_;
	ID3D11Buffer* matrixbuffer_;
	ID3D11SamplerState* samplestate_;
	std::unique_ptr<Support> support_;
public:
	Alphamapshader();
	~Alphamapshader();
	bool init();
	void destroy();
	bool render(int Indexcount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView**TextureArray);
};

