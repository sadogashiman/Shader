#pragma once
#include"TextureArray.h"
#include"texture.h"
class Model
{
private:
	struct Vertextype
	{
		Vector3 position;
		Vector2 texture;
		Vector3 normal;

	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

	ID3D11Buffer* vertexbuff_;
	ID3D11Buffer* indexbuff_;

	int vertexcount_;
	int indexcount_;

	bool initbuff();
	void destroybuff();
	void renderbuff();

	bool Loadtexture(const wchar_t* FileName);
	void Releasetexture();

	bool LoadModel(const wchar_t* FileName);
	void ReleaseModel();
	TextureArray* texturearray_;
	Texture* texture_;
	ModelType* model_;
	float positionx, positiony, positionz;

public:
	Model();
	~Model();
	bool init(const wchar_t* TextureFileName, const wchar_t* ModelFileName);
	void destroy();
	void render();

	//set
	void setPosition(const float X, const float Y, const float Z);

	//get
	void getPosition(float& X, float& Y, float& Z);
	const int getIndexCount()const;
	ID3D11ShaderResourceView* getTexture();

};

