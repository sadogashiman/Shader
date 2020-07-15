#pragma once
#include"Support.h"
#include"Direct3D.h"

class SkyplaneShader
{
private:
	struct SkyBufferType
	{
		float transition;
		float scale;
		float btight;
		float padding;
	};

	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};
	Direct3D* instanceptr_;
	ComPtr<ID3D11VertexShader> vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout> layout_;
	ComPtr<ID3D11SamplerState> sampler_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	ComPtr<ID3D11Buffer> skybuffer_;
	std::unique_ptr<Support> support_;

	bool setShaderParameters(Matrix World,Matrix View,Matrix Projection,ID3D11ShaderResourceView* Texture1,ID3D11ShaderResourceView* Texture2,float Transition,float Scale,float Bright);
	void renderShader(const int IndexCount);
public:
	SkyplaneShader();
	~SkyplaneShader();
	bool init();
	bool render(const int IndexCount,Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1, ID3D11ShaderResourceView* Texture2, float Transition, float Scale, float Bright);
};

