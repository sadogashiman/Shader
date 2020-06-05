#pragma once
#include"Support.h"
class Skydomeshader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	struct GradientBufferType
	{
		Vector4 apexcolor;
		Vector4 centorcolor;
	};
	ComPtr<ID3D11VertexShader>vertexshader_;
	ComPtr<ID3D11PixelShader>pixelshader_;
	ComPtr<ID3D11InputLayout>layout_;
	ComPtr<ID3D11Buffer>matrixbuffer_;
	ComPtr<ID3D11Buffer>gradientbuffer_;
	std::unique_ptr<Support> support_;

	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, Vector4 ApexColor, Vector4 CentorColor);
	void renderShader(const int IndexCount);

public:
	Skydomeshader();
	~Skydomeshader();

	bool init();
	bool render(const int Indexcount, Matrix World, Matrix View, Matrix Projection, Vector4 ApexColor, Vector4 CentorColor);
};

