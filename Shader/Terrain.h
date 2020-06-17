#pragma once
#include"Model3D.h"
#include"TextureFactory.h"
class Terrain:public Model3D
{
private:
	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
	};

	struct HeightMapType
	{
		float x;
		float y;
		float z;
	};

	struct ModelType
	{
		float x;
		float y;
		float z;
		float u;
		float v;
	};

	ComPtr<ID3D11Buffer> vertexbuffer_;
	ComPtr<ID3D11Buffer> indexbuffer_;
	int vertexcnt_;
	int indexcnt_;
	int terrainheight_;
	int terrainwidth_;
	float heightscale_;
	char terrainfilename_[MAX_PATH];
	wchar_t texturefilename_[MAX_PATH];
	std::vector<HeightMapType> heightmap_;
	std::vector<ModelType> model_;

	bool initbuffer();
	bool loadTerrainDataFile(const wchar_t* FileName);
	bool loadBitmapHeightMap();
	void setTerrainCoordinate();
	bool buildTerrainModel();

	void destroyHightMap();
	void destroyTerrainModel();
public:
	Terrain();
	~Terrain();
	bool init(const wchar_t* ModelFileName = nullptr, const wchar_t* TextureFileName = nullptr);
	void render();
	void destroy();
	
	//get
	inline const int getIndexCount()const { return indexcnt_; }
	inline ID3D11ShaderResourceView* getTexture() const { return TextureFactory::getInstance()->getTexture(texturefilename_); }

};

