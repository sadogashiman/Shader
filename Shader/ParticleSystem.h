#pragma once
#include"TextureFactory.h"
const int kFrameTime = 16;
class ParticleSystem
{
private:
	struct ParticleType
	{
		Vector3 position;
		Vector3 color;
		float velocity;
		bool active;


	};

	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
		Vector4 color;
	};

	struct ParticleData
	{
		Vector3 deviation;
		float velocity;
		float velocityVariation;
		float size;
		float perSecond;
		int max;
	};



	bool initParticleSystem();

	bool initbuffer();
	bool updateBuffer();
	void renderBuffer();

	void emitParticle();
	void updateParticle();
	void killParticle();

	int currentcnt_;
	float accumulatedtime_;
	std::vector<ParticleType> particlevector_;
	int vertexcnt_;
	int indexcnt_;
	std::vector<VertexType> vertices_;
	ComPtr<ID3D11Buffer> vertexbuffer_;
	ComPtr<ID3D11Buffer> indexbuffer_;
	ParticleData data_;
	wchar_t filename_[MAX_PATH];
public:
	ParticleSystem();
	~ParticleSystem();
	bool init(const wchar_t* FileName);
	bool update();
	void render();
	void destroy();

	//Get
	inline ID3D11ShaderResourceView* getTexture()const { return TextureFactory::getInstance()->getTexture(filename_); }
	inline int getIndexCount()const { return indexcnt_; }
};

