#pragma once
#include"Lightshader.h"
#include"model.h"
#include"OrthoWindow.h"
#include"Deferredbuffers.h"
#include"Deferredshader.h"
#include"Camera.h"
#include"Light.h"
#include"RayTrace.h"
#include"Ray_trace_HW.h"
#include"SkyDome.h"
#include"State.h"
#include"Terrain.h"
#include"Timer.h"

class Game :public State
{
private:

	bool renderSceneToTexture();
	Light* light_;
	Model* model_;
	Camera* camera_;
	OrthoWindow* ortho_;
	Deferredbuffers* defbuffer_;
	LightShader* lightshader_;
	RayTrace* raytrace_;
	Ray_trace_HW* rayhw_;
	SkyDome* sky_;
	Terrain* terrain_;
	Position position_;
	void handleMovementInput();
	bool wire_;
public:
	Game();
	~Game();
	bool init();
	State* update();
	bool render();
	void destroy();
};

