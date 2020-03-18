#pragma once
#include"Support.h"

class Depthshader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};
	bool setShaderParameters(Matrix worldMatrix, Matrix viewMatrix, Matrix projectionMatrix);
	void renderShader(int indexCount);

	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	ID3D11InputLayout* layout_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	std::unique_ptr<Support> support_;

public:
	Depthshader();
	~Depthshader();
	bool init();
	void destroy();
	bool render(int indexCount, Matrix worldMatrix, Matrix viewMatrix, Matrix projectionMatrix);
};

