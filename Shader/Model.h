#pragma once
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

	bool Loadtexture(std::filesystem::path FileName);
	void Releasetexture();

	bool LoadModel(std::filesystem::path FileName);
	void ReleaseModel();
	ID3D11ShaderResourceView* texture_;
	ModelType* model_;
	float positionx, positiony, positionz;
	wchar_t* filename_;
public:
	Model();
	~Model();
	bool init(std::filesystem::path TextureFileName, std::filesystem::path ModelFileName);
	void destroy();
	void render();

	//set
	void setPosition(const float X, const float Y, const float Z);

	//get
	void getPosition(float& X, float& Y, float& Z);
	inline const int getIndexCount()const {return indexcount_;}
	inline ID3D11ShaderResourceView* getTexture()const { return texture_; }

};

