#pragma once
#include"State.h"
class System
{
private:
	bool update();
	void initWindows(int& ScreenWidth, int& ScreenHeight);
	void destroyWindows();
	static int screenwidth_;
	static int screenheight_;
	std::unique_ptr<State> state_;

protected:
	HINSTANCE instance_;
	static HWND hwnd_;
public:
	System();
	~System();
	bool init();
	bool run();
	void destroy();
	bool handleInput(const float FrameTime);
	
	//get
	static const HWND getWindowHandle() { return hwnd_; }
	static inline const int getWindowWidth() { return screenwidth_; }
	static inline const int getWindowHeight(){ return screenheight_; }



	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
static LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static System* AppHandle = nullptr;