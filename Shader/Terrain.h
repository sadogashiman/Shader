#pragma once
#include"Model3D.h"
class Terrain:public Model3D
{
private:
	struct VertexType
	{
		Vector3 position;
		Vector4 color;
	};

	ComPtr<ID3D11Buffer> vertexbuffer_;
	ComPtr<ID3D11Buffer> indexbuffer_;
	int vertexcnt_;
	int indexcnt_;
public:
	Terrain();
	~Terrain();
	bool init(const wchar_t* ModelFileName = nullptr);
	void render();
	void destroy();
	
	//get
	inline const int getIndexCount()const { return indexcnt_; }

};

