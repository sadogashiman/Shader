#pragma once

class FontShader
{
private:
	struct MatrixBufferType
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	struct PixelBufferType
	{
		Vector4 pixelcolor;
	};

	ComPtr<ID3D11VertexShader> vertexshader_;
	ComPtr<ID3D11PixelShader> pixelshader_;
	ComPtr<ID3D11InputLayout> layout_;
	ComPtr<ID3D11Buffer> matrixbuffer_;
	ComPtr<ID3D11Buffer> pixelbuffer_;
	ComPtr<ID3D11SamplerState> samplestate_;
	
	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Vector4 Color);
	void renderShader(const int IndexCount);
public:
	FontShader();
	~FontShader();
	bool init();
	bool render(const int IndexCount, Matrix World, Matrix View, Matrix Projection, ID3D11ShaderResourceView* Texture, Vector4 Color);
};

