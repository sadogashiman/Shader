#pragma once
#include"Model3D.h"
#include"TextureFactory.h"
class Skyplane:public Model3D
{
private:
	struct SkyPlaneType
	{
		Vector3 position;
		Vector2 texture;
	};

	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
	};

	std::vector<SkyPlaneType>skyplane_;
	int indexcount_;
	int vertexcount_;
	ComPtr<ID3D11Buffer> vertexbuffer_;
	ComPtr<ID3D11Buffer> indexbuffer_;
	wchar_t texfilename1_[MAX_PATH];
	wchar_t texfilename2_[MAX_PATH];
	float bright_;
	float transspeed_[4];
	float texposition_[4];

	bool initskyPlane(const int PlaneResolution, const float PlaneWidth, const float PlaneTop, const float PlaneBottom, const int texRepeat);


public:
	Skyplane();
	~Skyplane();
	bool init(const wchar_t* TextureFileName1, const wchar_t* TextureFileName2);
	void render();
	void destroy();

	//get
	inline const int getIndexCount()const { return indexcount_; }
	inline const float getCloudBright()const { return bright_; }
	inline const float getTranslation(const int Index) { return texposition_[Index]; }
	inline ID3D11ShaderResourceView* getTexture1() { return TextureFactory::getInstance()->getTexture(texfilename1_); }
	inline ID3D11ShaderResourceView* getTexture2() { return TextureFactory::getInstance()->getTexture(texfilename2_); }
};

