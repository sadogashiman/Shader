#pragma once
#include"TextureFactory.h"
#include"Model3D.h"
class Model:public Model3D
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

	ComPtr<ID3D11Buffer> vertexbuff_;
	ComPtr<ID3D11Buffer> indexbuff_;

	int vertexcount_;
	int indexcount_;
	int facecount_;

	//init
	bool initBuffer();

	//load
	void loadTexture(const wchar_t* FileName);
	bool loadModel(const wchar_t* ModelFileName);

	//release
	void releaseTexture();

	//render
	void renderBuffer();

	std::vector<ModelType> model_;
	float positionx_, positiony_, positionz_;
	wchar_t texturefilename_[MAX_PATH];
	wchar_t mapfilename_[MAX_PATH];
	ID3D11ShaderResourceView* texturearray_[3];
public:
	Model();
	~Model();
	bool init(const wchar_t* ModelFileName,const wchar_t* TextureFileName = nullptr);
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

