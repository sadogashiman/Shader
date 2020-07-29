#pragma once
#include"Support.h"
#include"Direct3D.h"
class SpotlightShader
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
		Vector3 position;
		float range;
		Vector4 diffusecolor;
		Vector4 ambientcolor;
		Vector3 direction;
		float cone;
		Vector3 attenuator_;
		float padding;
	};


	ComPtr<ID3D11VertexShader> vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout> layout_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	ComPtr<ID3D11Buffer> lightbuffer_;
	ComPtr<ID3D11SamplerState> sampler_;
	std::unique_ptr<Support> support_;
	Direct3D* instanceptr_;

	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Vector3 LightPosition, float Range, Vector4 DiffuseColor, Vector4 AmbientColor, Vector3 LightDirection, float Cone, Vector3 Att);
	void renderShader(const int IndexCount);

public:
	SpotlightShader();
	~SpotlightShader();
	bool init();
	bool render(const int IndexCount, Matrix World, Matrix View, Matrix Projection,ID3D11ShaderResourceView* Texture, Vector3 LightPosition, float Range, Vector4 DiffuseColor, Vector4 AmbientColor, Vector3 LightDirection, float Cone, Vector3 Att);
};

