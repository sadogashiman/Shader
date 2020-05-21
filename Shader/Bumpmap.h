#pragma once
#include"Support.h"

class Bumpmap
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	struct LightBufferType
	{
		Vector4 diffuseColor;
		Vector3 lightDirection;
		float padding;
	};

	bool SetShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView** TextureArray, Vector3 LightDirection, Vector4 DiffuseColor);
	void renderShader( const int Indexcount);

	ComPtr<ID3D11VertexShader> vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout> layout_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	ComPtr<ID3D11SamplerState> samplerstate_;
	ComPtr<ID3D11Buffer> lightbuffer_;
	std::unique_ptr<Support> support_;

public:
	Bumpmap();
	~Bumpmap();
	bool init();
	bool render(const int Indexcound, const Matrix World, const Matrix View, const Matrix Projection, ID3D11ShaderResourceView** TextureArray, Vector3 LightDirection, Vector4 DiffuseColor);

};

