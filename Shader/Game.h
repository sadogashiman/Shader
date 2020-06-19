#pragma once
#include"Lightshader.h"
#include"model.h"
#include"OrthoWindow.h"
#include"Deferredbuffers.h"
#include"Deferredshader.h"
#include"Camera.h"
#include"Light.h"
#include"SkyDome.h"
#include"State.h"
#include"Terrain.h"
#include"Timer.h"
#include"OrthoWindow.h"

class Game :public State
{
private:

	Light* light_;
	Model* model_;
	Camera* camera_;
	OrthoWindow* ortho_;
	Deferredbuffers* defbuffer_;
	LightShader* lightshader_;
	SkyDome* sky_;
	Terrain* terrain_;
	OrthoWindow* orthowindow_;
	Position position_;
	void handleMovementInput();
	bool renderSceneToTexture();

	bool wire_;
public:
	Game();
	~Game();
	bool init();
	State* update();
	bool render();
	void destroy();
};

