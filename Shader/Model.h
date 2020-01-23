#pragma once
#include"TextureFactory.h"
enum MappingType
{
	kMaskMap,
	kBumpMap,
};
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
	int facecount_;

	bool initBuffer();
	void renderBuffer();
	void loadTexture(const wchar_t* FileName);
	void releaseTexture();
	void releaseModel();

	ModelType* model_;
	std::vector<Vector3> vertexvec_;
	std::vector<Vector2> texturevec_;
	std::vector<Vector3> normalvec_;
	float positionx_, positiony_, positionz_;
	wchar_t texturefilename_[MAX_PATH];
	wchar_t mapfilename_[MAX_PATH];
	ID3D11ShaderResourceView* texturearray_[3];
public:
	Model();
	~Model();
	bool init(const wchar_t* TextureFileName, const wchar_t* ModelFileName,MappingType Type,const wchar_t* TextureFileName2 = nullptr);
	void destroy();
	void render();

	//set
	void setPosition(const float X, const float Y, const float Z);

	//get
	void getPosition(float& X, float& Y, float& Z);
	inline const int getIndexCount()const { return indexcount_; }
	inline ID3D11ShaderResourceView* getTexture()const { return TextureFactory::getInstance()->getTexture(texturefilename_); }
	inline ID3D11ShaderResourceView* getNormalTexture()const { return TextureFactory::getInstance()->getTexture(mapfilename_); }
	inline ID3D11ShaderResourceView** getTextureMapArray() { return texturearray_; }
};

