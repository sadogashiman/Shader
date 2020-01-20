#pragma once
#include"TextureFactory.h"
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

	bool initBuffer();
	void destroybuff();
	void renderBuffer();

	void releaseTexture();

	bool loadModel(const wchar_t* FileName);
	void releaseModel();
	ModelType* model_;
	float positionx, positiony, positionz;
	wchar_t texturefilename_[MAX_PATH];
	wchar_t normalfilename_[MAX_PATH];
public:
	Model();
	~Model();
	bool init(const wchar_t* TextureFileName, const wchar_t* ModelFileName,const wchar_t* NormalFileName=nullptr);
	void destroy();
	void render();

	//set
	void setPosition(const float X, const float Y, const float Z);

	//get
	void getPosition(float& X, float& Y, float& Z);
	inline const int getIndexCount()const {return indexcount_;}
	inline ID3D11ShaderResourceView* getTexture()const { return TextureFactory::getInstance()->getTexture(texturefilename_); }
	inline ID3D11ShaderResourceView* getNormalTexture()const { return TextureFactory::getInstance()->getTexture(normalfilename_); }

};

