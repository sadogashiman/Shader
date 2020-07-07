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
	wchar_t cloudtexfilename_[MAX_PATH];
	wchar_t perturbtexfilename_[MAX_PATH];
	float bright_;
	float scale_;
	float transition_;
	Matrix world_;
	Vector3 position_;
	Vector3 rotation_;

	bool initskyPlane(const int PlaneResolution, const float PlaneWidth, const float PlaneTop, const float PlaneBottom, const int texRepeat);


public:
	Skyplane();
	~Skyplane();
	bool init(const wchar_t* TextureFileName1, const wchar_t* TextureFileName2);
	void render();
	void destroy();

	//set
	inline void setCloudBrightness(const float Brightness) { bright_ = Brightness; }
	inline void setCloudScale(const float CloudScale) { scale_ = CloudScale; }

	//get
	Matrix getWorldMatrix();
	inline const int getIndexCount()const { return indexcount_; }
	inline const float getBrightness()const { return bright_; }
	inline const float getTransition()const { return transition_; }
	inline const float getScale()const { return scale_; }
	inline ID3D11ShaderResourceView* getCloudTexture() { return TextureFactory::getInstance()->getTexture(cloudtexfilename_); }
	inline ID3D11ShaderResourceView* getPerturbTexture() { return TextureFactory::getInstance()->getTexture(perturbtexfilename_); }
};

