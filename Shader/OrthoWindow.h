#pragma once
class OrthoWindow
{
private:
	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
	};
	ID3D11Buffer* vertexbuffer_;
	ID3D11Buffer* indexbuffer_;
	int vertexcnt_;
	int indexcnt_;

public:
	bool init(const float WindowWidth, const float WindowHeight);
	void render();
	void destroy();
	inline int getIndexCount()const { return indexcnt_; }
};

