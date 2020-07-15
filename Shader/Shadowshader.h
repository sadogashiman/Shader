#pragma once
#include"Support.h"
#include"Direct3D.h"

class Shadowshader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
		Matrix lightview;
		Matrix lightprojection;
	};

	struct LightBufferType
	{
		Vector4 ambientColor;
		Vector4 diffuseColor;
	};

	struct LightBufferType2
	{
		Vector3 lightposition;
		float padding;
	};

	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, Matrix lightview, Matrix Lightprojection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 LightDirection, Vector4 Ambientcolor, Vector4 Diffusecolor);
	void renderShader(const int Indexcount);

	Direct3D* instanceptr_;
	ComPtr<ID3D11VertexShader> vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout> layout_;
	ComPtr<ID3D11SamplerState> samplestateclamp_;
	ComPtr<ID3D11SamplerState> samplestatewrap_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	ComPtr<ID3D11Buffer> lightbuffer_;
	ComPtr<ID3D11Buffer> lightbuffer2_;
	std::unique_ptr<Support> support_;

public:
	Shadowshader();
	~Shadowshader();
	bool init();
	bool render(const int Indexcound,Matrix World,Matrix View,Matrix Projection,Matrix lightview, Matrix lightprojection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 LightDirection, Vector4 Ambientcolor, Vector4 Diffusecolor);
};

