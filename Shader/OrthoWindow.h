#pragma once
#include"Direct3D.h"
class OrthoWindow
{
private:
	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
	};

	Direct3D* instanceptr_;
	ComPtr<ID3D11Buffer> vertexbuffer_;
	ComPtr<ID3D11Buffer> indexbuffer_;
	int vertexcnt_;
	int indexcnt_;

public:
	OrthoWindow();
	~OrthoWindow();
	bool init(const float WindowWidth, const float WindowHeight);
	void render();
	inline int getIndexCount()const { return indexcnt_; }
};

