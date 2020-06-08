#pragma once
class Terrain
{
private:
	struct VertexType
	{
		Vector3 position;
		Vector4 color;
	};

	ID3D11Buffer* vertexbuffer_;
	ID3D11Buffer* indexbuffer_;
	int vertexcnt_;
	int indexcnt_;
public:
	Terrain();
	~Terrain();
	bool init();
	bool render();
	void destroy();
	
	//get
	inline const int getIndexCount()const { return indexcnt_; }

};

