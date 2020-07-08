#pragma once
#include"Support.h"

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
		Vector4 AmbientColor;
		Vector4 DiffuseColor;
		Vector3 Lightdirection;
		float padding;
	};



	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, Matrix lightview, Matrix Lightprojection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 lightposition, Vector4 Ambientcolor, Vector4 Diffusecolor,Vector3 Direction);
	void renderShader(const int Indexcount);
	ComPtr<ID3D11VertexShader> vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout> layout_;
	ComPtr<ID3D11SamplerState> samplestateclamp_;
	ComPtr<ID3D11SamplerState> samplestatewrap_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	ComPtr<ID3D11Buffer> lightbuffer_;
	std::unique_ptr<Support> support_;

public:
	Shadowshader();
	~Shadowshader();
	bool init();
	bool render(const int Indexcound, const Matrix World, const Matrix View, const Matrix Projection, const Matrix lightview, const Matrix lightprojection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 lightposition, Vector4 Ambientcolor, Vector4 Diffusecolor, Vector3 Direction);
};

