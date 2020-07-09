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
	Position position_;
	Rendertexture* rendertexture_;

	//3Dモデル
	Terrain* terrain_;
	SkyDome* sky_;
	Skyplane* cloud_;

	Model* bill03_;
	Model* bill04_;
	Model* bill06_;
	Model* bill10_;
	Model* ground_;


	//パーティクル
	ParticleSystem* particle_;

	bool wire_;
	void switchWireFrame();
	bool modelRender();
	bool worldRender();
	bool renderToScene();
public:
	Game();
	~Game();
	bool init();
	State* update();
	bool render();
	void destroy();
};

