#pragma once
//g‚í‚ê‚Ä‚¢‚È‚¢ˆê•”‚ÌAPI‚ğœŠO
#define WIN32_LEAN_AND_MEAN


class System
{
private:
	bool update();
	void initWindows(int& ScreenWidth, int& ScreenHeight);
	void destroyWindows();
protected:
	HINSTANCE instance_;
	HWND hwnd_;
public:
	System();
	~System();
	bool init();
	bool run();
	void destroy();
	bool handleInput(const float FrameTime);
	LRESULT CALLBACK MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
static LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static System* AppHandle = nullptr;