#pragma once
#include"model.h"
#include"Camera.h"
#include"Light.h"
#include"SkyDome.h"
#include"State.h"
#include"Terrain.h"
#include"Skyplane.h"
#include"Rendertexture.h"
#include"ParticleSystem.h"
class Game :public State
{
private:
	Light* light_;
	Camera* camera_;
	SkyDome* sky_;
	Terrain* terrain_;
	Position position_;
	Skyplane* cloud_;
	Rendertexture* rendertexture_;

	//3Dモデル
	Model* texmodel_;
	Model* bumpmodel_;
	Model* shadowmodel_;
	Model* maskmodel_;
	Model* multimodel_;

	//パーティクル
	ParticleSystem* particle_;

	bool wire_;

	void switchWireFrame();
	bool modelRender();
	bool worldRender();
public:
	Game();
	~Game();
	bool init();
	State* update();
	bool render();
	void destroy();
};

