#pragma once
#include "Support.h"
class SkyDom
{
private:
	SkyDom() = default;
	~SkyDom() = default;

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

	bool init(const wchar_t* ModelFileName);
	bool render();
	void destroy();

	//get
	inline const int getIndexCount()const { return indexcount_; }
	inline const Vector4 getApexColor()const { return apexcolor_; }
	inline const Vector4 getCenterColor()const { return centercolor_; }

	static inline SkyDom* getInstance()
	{
		static SkyDom instance_;
		return &instance_;
	}



};

