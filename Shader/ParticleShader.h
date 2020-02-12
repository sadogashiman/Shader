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
	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	ID3D11InputLayout* layout_;
	ID3D11Buffer* matrixbuffer_;
	ID3D11SamplerState* samplerstate_;
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

