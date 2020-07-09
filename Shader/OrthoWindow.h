#pragma once
class OrthoWindow
{
private:
	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
	};
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

