#pragma once
#include"Model3D.h"
class SkyDome:public Model3D
{
private:
	struct ModelType
	{
		Vector3 position;
		Vector2 tex;
		Vector3 normal;
	};

	struct VertexType
	{
		Vector3 position;
	};

	ComPtr<ID3D11Buffer> vertexbuffer_;
	ComPtr<ID3D11Buffer> indexbuffer_;
	Vector4 apexcolor_;
	Vector4 centercolor_;
	std::vector<ModelType> model_;
	int indexcount_;
	int vertexcount_;

	bool loadSkyDomModel(const wchar_t* ModelFileName);
	void releaseSkyDomModel();

public:
	SkyDome();
	~SkyDome();
	bool init(const wchar_t* ModelFileName);
	void render();
	void destroy();

	//set
	inline void setApexColor(const Vector4& ApexColor) { apexcolor_ = ApexColor; }
	inline void setApexColor(const float Red, const float Green, const float Blue, const float Alpha) { apexcolor_ = Vector4(Red, Green, Blue, Alpha); }
	inline void setCentorColor(const Vector4& CentorColor) { centercolor_ = CentorColor; }
	inline void setCentorColor(const float Red, const float Green, const float Blue, const float Alpha) { centercolor_ = Vector4(Red, Green, Blue, Alpha); }

	//get
	inline const int getIndexCount()const { return indexcount_; }
	inline const Vector4 getApexColor()const { return apexcolor_; }
	inline const Vector4 getCenterColor()const { return centercolor_; }
};

