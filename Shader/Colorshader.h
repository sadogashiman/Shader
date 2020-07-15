#pragma once
#include"Support.h"
#include "Direct3D.h"

class Colorshader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	struct ColorBufferType
	{
		Vector4 color;
	};

	Direct3D* instanceptr_;
	ComPtr<ID3D11VertexShader>vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout>layout_;
	ComPtr<ID3D11Buffer>matrixbuffer_;
	ComPtr<ID3D11Buffer> colorbuffer_;
	std::unique_ptr<Support> support_;

	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, Vector4 Color);
	void renderShader(const int IndexCount);

public:
	Colorshader();
	~Colorshader();
	bool init();
	bool render(const int IndexCount, Matrix World, Matrix View, Matrix Projection,Vector4 Color);
};

