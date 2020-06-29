#pragma once
#include"TextureFactory.h"
const int kFrameTime = 16;
const float kAlphaAttenuation = 0.001F;
const float kPositionAttenuation = 0.001F;
struct ParticleData
{
	Vector3 deviation;
	float deviationrange;
	float velocity;
	float velocityVariation;
	float size;
	float perSecond;
	unsigned int max;
	float colorbase;
	float colorrange;
	float alphabase;
	float alpharange;
	float deleteline;
};

class ParticleSystem
{
private:
	struct ParticleType
	{
		Vector3 position;
		Vector4 color;
		float velocity;
		bool active;
	};

	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
		Vector4 color;
	};

	bool initParticleSystem();

	bool initbuffer();
	bool updateBuffer();
	void renderBuffer();

	void emitParticle();
	void updateParticle();
	void killParticle();

	int vertexcnt_;
	int indexcnt_;

	float accumulatedtime_;

	std::vector<VertexType> vertices_;
	std::vector<ParticleType> particlevector_;
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

	//set
	//個別設定
	inline void setEmitPosition(const Vector3& EmitPosition) { data_.deviation = EmitPosition; }
	inline void setEmitPosition(const float EmitPosX, const float EmitPosY, const float EmitPosZ) { data_.deviation = Vector3(EmitPosX, EmitPosY, EmitPosZ); }
	inline void setVelcity(const float Velocity) { data_.velocity = Velocity; }
	inline void setVelocityVariation(const float Variation) { data_.velocityVariation = Variation; }
	inline void setParticleSize(const float Size) { data_.size = Size; }
	inline void setEmitMax(const int Max) { data_.max = Max; }
	//まとめて設定
	inline void setParticleData(const ParticleData& ParticleData) { data_ = ParticleData; }

	//Get
	inline ID3D11ShaderResourceView* getTexture()const { return TextureFactory::getInstance()->getTexture(filename_); }
	inline int getIndexCount()const { return indexcnt_; }
};

