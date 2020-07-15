#pragma once
#include"Support.h"
#include "Direct3D.h"

class TerrainShader
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
		Vector4 ambientcolor;
		Vector4 diffusecolor;
		Vector3 lightdirection;
		float padding;
	};
	Direct3D* instanceptr_;
	ComPtr<ID3D11VertexShader> vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout> layout_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	ComPtr<ID3D11Buffer> lightbuffer_;
	ComPtr<ID3D11SamplerState>samplestate_;
	std::unique_ptr<Support> support_;


	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, Vector4 AmbientColor, Vector4 DiffuseColor, Vector3 LightDirection, ID3D11ShaderResourceView* Texture);
	void renderShader(const int IndexCount);

public:
	TerrainShader();
	~TerrainShader();
	bool init();
	bool render(const int IndexCount, Matrix World, Matrix View, Matrix Projection, Vector4 AmbientColor, Vector4 DiffuseColor, Vector3 LightDirection,ID3D11ShaderResourceView* Texture);

};

