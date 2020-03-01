#pragma once
#include "Support.h"

class FontShader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	struct PixelBufferType
	{
		Vector4 pixelcolor;
	};

	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	ID3D11InputLayout* layout_;
	ID3D11Buffer* matrixbuffer_;
	ID3D11Buffer* pixelbuffer_;
	ID3D11SamplerState* samplestate_;
	std::unique_ptr<Support> support_;

	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Vector4 Color);
	void renderShader(const int IndexCount);
public:
	FontShader();
	~FontShader();
	bool init();
	bool render(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Vector4 Color);
};

