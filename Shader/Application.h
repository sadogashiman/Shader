#pragma once
#include"Lightshader.h"
#include"model.h"
#include"OrthoWindow.h"
#include"Deferredbuffers.h"
#include"Deferredshader.h"
#include"Camera.h"
#include"Light.h"

class Application
{
private:
	Application();
	~Application();
	bool renderSceneToTexture();
	Light* light_;
	Model* model_;
	Camera* camera_;
	OrthoWindow* ortho_;
	Deferredbuffers* defbuffer_;
	Deferredshader* defshader_;
	LightShader* lightshader_;

public:
	Application(const Application&) = delete;
	Application& operator = (const Application&) = delete;
	Application(Application&&) = delete;
	Application& operator = (Application&&) = delete;

	bool init(HWND Hwnd, const int ScreenWidth, const int ScreenHeight);
	bool update();
	bool render();
	void destroy();

	static inline Application* getInstance()
	{
		static Application instance;
		return &instance;
	}
};

