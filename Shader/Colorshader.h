#pragma once
#include"Support.h"
class Colorshader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	ComPtr<ID3D11VertexShader>vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout>layout_;
	ComPtr<ID3D11Buffer>matrixbuffer_;
	std::unique_ptr<Support> support_;

	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection);
	void renderShader(const int IndexCount);

public:
	Colorshader();
	~Colorshader();
	bool init();
	bool render(const int IndexCount, Matrix World, Matrix View, Matrix Projection);
};

