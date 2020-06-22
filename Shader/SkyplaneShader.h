#pragma once
#include"Support.h"
class SkyplaneShader
{
private:
	struct SkyBufferType
	{
		float firsttransx;
		float firsttransz;
		float secondtransx;
		float secondtransz;
		float bright;
		Vector3 padding;
	};

	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	ComPtr<ID3D11VertexShader> vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout> layout_;
	ComPtr<ID3D11SamplerState> sampler_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	ComPtr<ID3D11Buffer> skybuffer_;
	std::unique_ptr<Support> support_;

	bool setShaderParameters(Matrix World,Matrix View,Matrix Projection,ID3D11ShaderResourceView* Texture1,ID3D11ShaderResourceView* Texture2,float FirsttransX,float FirsttransZ,float SecondtransX,float SecondtransZ,float Bright);
	void renderShader(const int IndexCount);
public:
	bool init();
	bool render(const int IndexCount,Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture1, ID3D11ShaderResourceView* Texture2, float FirsttransX, float FirsttransZ, float SecondtransX, float SecondtransZ, float Bright);
};

