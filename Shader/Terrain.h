#pragma once
class Terrain
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
	bool init();
	bool render();
	
	//get
	inline const int getIndexCount()const { return indexcnt_; }

};

