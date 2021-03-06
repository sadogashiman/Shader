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
#include "Direct3D.h"

class Game :public State
{
private:
	Light* light_;
	Camera* camera_;
	Position position_;
	Rendertexture* rendertexture_;
	Direct3D* instanceptr_;

	//3D���f��
	Terrain* terrain_;
	SkyDome* sky_;
	Skyplane* cloud_;
	std::vector<Model*> model_;

#ifdef _DEBUG
	Model* lightblock_;
		
#endif // _DEBUG

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

