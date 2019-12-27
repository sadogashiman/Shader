#pragma once
#include"error.h"
#include"release.h"
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

	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	ID3D11InputLayout* layout_;
	ID3D11Buffer* matrixbuff_;
	ID3D11SamplerState* samplerstate_;
	ID3D11Buffer* lightbuffer_;

public:
	LightShader();
	~LightShader();
	bool init();
	void destroy();
	bool render(const int Indexcound, const Matrix World, const Matrix View, const Matrix Projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* NormalTexture,Vector3 LightDirection);
};

