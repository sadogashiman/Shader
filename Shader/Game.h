#pragma once
#include"model.h"
#include"Camera.h"
#include"Light.h"
#include"SkyDome.h"
#include"State.h"
#include"Terrain.h"
#include"Skyplane.h"
#include"Tree.h"

class Game :public State
{
private:
	Light* light_;
	Camera* camera_;
	SkyDome* sky_;
	Terrain* terrain_;
	Position position_;
	Skyplane* skyplane_;

	//3Dƒ‚ƒfƒ‹
	Model* texmodel_;


	void switchWireFrame();
	bool wire_;
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

