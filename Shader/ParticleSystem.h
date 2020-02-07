#pragma once
#include"TextureFactory.h"
class ParticleSystem
{
private:
	struct ParticleType
	{
		Vector3 position;
		Vector3 color;
		float velocity;
		bool active;

		bool operator<(const ParticleType &particle)const
		{
			//ZŽ²‚ðŠî€‚É‘å¬‚ð”»’è
			return position.z < particle.position.z;
		}
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
	void destroyParticleSystem();

	bool initbuffer();
	bool updateBuffer();
	void renderBuffer();
	void destroyBuffer();

	void emitParticle(const float FrameTime);
	void updateParticle(const float FrameTime);
	void killParticle();

	int currentcnt_;
	float accumulatedtime_;
	std::list<std::unique_ptr<ParticleType>> particlelist_;
	int vertexcnt_;
	int indexcnt_;
	std::vector<VertexType> vertices_;
	ID3D11Buffer* vertexbuffer_;
	ID3D11Buffer* indexbuffer_;
	ParticleData data_;
	wchar_t filename_[MAX_PATH];
public:
	ParticleSystem();
	~ParticleSystem();
	bool init(const wchar_t* FileName);
	bool update(const float FrameTime);
	void render();
	void destroy();

	//Get
	inline ID3D11ShaderResourceView* getTexture()const { return TextureFactory::getInstance()->getTexture(filename_); }
	inline int getIndexCount()const { return indexcnt_; }
};

