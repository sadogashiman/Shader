#pragma once

class System
{
private:
	bool update();
	void initWindows(int& ScreenWidth, int& ScreenHeight);
	void destroyWindows();
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
	static const HWND getWindowHandle(){ return hwnd_; }
	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
static LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static System* AppHandle = nullptr;