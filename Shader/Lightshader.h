#pragma once

#include"Support.h"

class LightShader
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
		Vector3 lightDirection;
		float pading;
	};

	bool SetShaderParameters(Matrix World, Matrix View, Matrix Projection,ID3D11ShaderResourceView* texture,ID3D11ShaderResourceView* NormalTexture,Vector3 LightDirection);
	void renderShader(const int Indexcount);

	ComPtr<ID3D11VertexShader> vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout> layout_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	ComPtr<ID3D11SamplerState> samplerstate_;
	ComPtr<ID3D11Buffer> lightbuffer_;


public:
	LightShader();
	~LightShader();
	bool init();
	bool render(const int Indexcound, const Matrix World, const Matrix View, const Matrix Projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* NormalTexture,Vector3 LightDirection);
};

