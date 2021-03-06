#pragma once
#include"Support.h"
#include"Direct3D.h"

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

	Direct3D* instanceptr_;
	ComPtr<ID3D11VertexShader> vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout> layout_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	std::unique_ptr<Support> support_;

public:
	Depthshader();
	~Depthshader();
	bool init();
	bool render(int indexCount, Matrix worldMatrix, Matrix viewMatrix, Matrix projectionMatrix);
};

