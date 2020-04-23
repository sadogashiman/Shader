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

class Game
{
private:
	Game();
	~Game();
	bool renderSceneToTexture();
	Light* light_;
	Model* model_;
	Camera* camera_;
	OrthoWindow* ortho_;
	Deferredbuffers* defbuffer_;
	LightShader* lightshader_;
	RayTrace* raytrace_;
	Ray_trace_HW* rayhw_;

public:
	Game(const Game&) = delete;
	Game& operator = (const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator = (Game&&) = delete;

	bool init(HWND Hwnd, const int ScreenWidth, const int ScreenHeight);
	bool update();
	bool render();
	void destroy();

	static inline Game* getInstance()
	{
		static Game instance;
		return &instance;
	}
};

