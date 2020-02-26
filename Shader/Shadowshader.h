#pragma once

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
	};

	struct LightBufferType2
	{
		Vector3 lightposition;
		float padding1;
	};

	bool setShaderParameters(Matrix World, Matrix View, Matrix Projection, Matrix lightview, Matrix Lightprojection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* Depthmaptexture, Vector3 lightposition,Vector4 Ambientcolor,Vector4 Diffusecolor);
	void renderShader(const int Indexcount);
	ID3D11VertexShader* vertexshader_;
	ID3D11PixelShader* pixelshader_;
	ID3D11InputLayout* layout_;
	ID3D11SamplerState* samplestateclamp_;
	ID3D11SamplerState* samplestatewrap_;
	ID3D11Buffer* matrixbuffer_;
	ID3D11Buffer* lightbuffer_;
	ID3D11Buffer* lightbuffer2_;

public:
	Shadowshader();
	~Shadowshader();
	bool init();
	void destroy();
	bool render(const int Indexcound, const Matrix World, const Matrix View, const Matrix Projection,const Matrix lightview,const Matrix lightprojection, ID3D11ShaderResourceView* texture,ID3D11ShaderResourceView* Depthmaptexture,Vector3 lightposition, Vector4 Ambientcolor,Vector4 Diffusecolor);
};

