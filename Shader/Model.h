#pragma once
#include"TextureFactory.h"
#include"Model3D.h"
const int kMaxTexture = 2;

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
	bool loadModel(const wchar_t* ModelFileName);

	//render
	void renderBuffer();

	std::vector<ModelType> model_;
	Vector3 position_;
	Vector3 rotation_;
	Matrix world_;
	float scale_;
	wchar_t texturefilename_[2][MAX_PATH];
	wchar_t mapfilename_[MAX_PATH];
public:
	Model();
	~Model();
	bool init(const wchar_t* ModelFileName,const wchar_t* TextureFileName = nullptr);
	bool init(const wchar_t* ModelFileName, const wchar_t* TextureFileName1, const wchar_t* TextureFileName2);
	bool init(const wchar_t* ModelFileName, const wchar_t* TextureFileName1, const wchar_t* TextureFileName2, const wchar_t* MaskFileName);

	void destroy();
	void render();

	//set
	inline void setPosition(const float X, const float Y, const float Z) { position_ = Vector3(X, Y, Z); }
	inline void setPosition(const Vector3& Position) {position_ = Position;}
	inline void setRotation(const float X, const float Y, const float Z) { rotation_ = Vector3(X, Y, Z); }
	inline void setRotation(const Vector3& Rotation) { rotation_ = Rotation; }
	inline void setNormalTexture(const wchar_t* NormalTextureFileName) { wcscpy(mapfilename_, NormalTextureFileName); } //マッピング用のテクスチャファイルは別途指定
	inline void setModelScale(const float Scale) { scale_ = Scale; }
																														//get
	Matrix getWorldMatrix();
	inline Vector3 getPosition()const { return position_; }
	inline Vector3 getRotation()const { return rotation_; }
	inline const int getIndexCount()const { return indexcount_; }
	inline ID3D11ShaderResourceView* getTexture(const int TextureNumber = 0)const { return TextureFactory::getInstance()->getTexture(texturefilename_[TextureNumber]); }
	inline ID3D11ShaderResourceView* getMapTexture()const { return TextureFactory::getInstance()->getTexture(mapfilename_); }
};

