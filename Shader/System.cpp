#include "stdafx.h"
#include "System.h"
#include "Direct3D.h"
#include "Input.h"
#include "ShaderManager.h"
#include"Game.h"
#include"TextureFactory.h"
HWND System::hwnd_;

System::System()
{
	instance_ = 0;
	hwnd_ = 0;
}

System::~System()
{
}

bool System::init()
{
	int screenwidth, screenheight;

	screenheight = 0;
	screenwidth = 0;

	//WindowsAPIを初期化
	initWindows(screenwidth, screenheight);

	//DirectX初期化
	if (!Direct3D::getInstance()->init(screenwidth, screenheight, kvsync, hwnd_, kFullScreen, kScreen_depth, kScreen_near))
	{
		Error::showDialog("DirectXの初期化に失敗");
		return false;
	}

	//インプットクラス初期化
	if (!Input::getInstance()->init(instance_, hwnd_))
	{
		Error::showDialog("DirectInputの初期化に失敗");
		return false;
	}

	//シェーダーマネージャー初期化
	if (!ShaderManager::getInstance()->init())
	{
		Error::showDialog("レンダラークラスの初期化に失敗");
		return false;
	}


	//ゲームクラス初期化
	if (!Game::getInstance()->init(hwnd_, screenwidth, screenheight))
	{
		Error::showDialog("ゲームクラスの初期化に失敗");
		return false;
	}

	return true;
}

bool System::run()
{
	MSG message;

	//メッセージ構造体初期化
	ZeroMemory(&message, sizeof(MSG));

	//メインループ
	while (1)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		if (message.message == WM_QUIT)
		{
			break;
		}
		else
		{
			if (!update())
			{
				return false;
			}
		}
	}

	return true;
}

void System::destroy()
{
	Game::getInstance()->destroy();
	TextureFactory::getInstance()->allDeleteTexture();
	ShaderManager::getInstance()->destroy();
	Input::getInstance()->destroy();
	Direct3D::getInstance()->destroy();
	destroyWindows();
}

bool System::update()
{
	bool result;

	Input::getInstance()->update();

	if (Input::getInstance()->isPressed(DIK_ESCAPE))
	{
		return false;
	}

	result = Game::getInstance()->update();
	if (!result)
	{
		return false;
	}


	return true;
}

void System::initWindows(int& ScreenWidth, int& ScreenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmscreensettings;

	//このオブジェクトへの外部ポインタを取得
	AppHandle = this;

	//アプリケーションインスタンスを取得
	instance_ = GetModuleHandle(NULL);

	//Window構造体初期化
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	//Windowクラスを設定
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WinProc;
	wc.hInstance = instance_;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = "ClassName";
	wc.cbSize = sizeof(WNDCLASSEX);

	//Windowクラスを登録
	RegisterClassEx(&wc);

	//デスクトップ画面解像度を取得
	RECT rec;
	HWND desktop = GetDesktopWindow();
	GetWindowRect(desktop, &rec);

	ScreenHeight = rec.right;
	ScreenWidth = rec.bottom;

	if (kFullScreen)
	{
		memset(&dmscreensettings, 0, sizeof(dmscreensettings));

		//全画面表示に設定
		dmscreensettings.dmSize = sizeof(dmscreensettings);
		dmscreensettings.dmPelsHeight = static_cast<unsigned long>(ScreenHeight);
		dmscreensettings.dmPelsWidth = static_cast<unsigned long>(ScreenWidth);
		dmscreensettings.dmBitsPerPel = 32;
		dmscreensettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//全画面表示に設定
		ChangeDisplaySettings(&dmscreensettings, CDS_FULLSCREEN);
	}
	else
	{
		//ウィンドウモード
		ScreenHeight = kWindow_Height;
		ScreenWidth = kWindow_Width;
	}

	//ウィンドウサイズを設定
	const DWORD style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
	const DWORD ex_style = WS_EX_OVERLAPPEDWINDOW;

	//誤差を修正
	AdjustWindowRectEx(&rec, style, false, ex_style);

	hwnd_ = CreateWindowEx(
		ex_style,
		"ClassName",
		"DirectX",
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		ScreenWidth,
		ScreenHeight,
		NULL,
		NULL,
		instance_,
		NULL
	);

	//ウィンドウを表示
	ShowWindow(hwnd_, SW_SHOW);
	SetForegroundWindow(hwnd_);
	SetFocus(hwnd_);

#ifdef _DEBUG
	ShowCursor(true);
#else
	ShowCursor(false);
#endif // DEBUG
}

void System::destroyWindows()
{
	ShowCursor(true);

	if (kFullScreen)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(hwnd_);

	hwnd_ = NULL;
	instance_ = NULL;
	AppHandle = NULL;
}

bool System::handleInput(const float FrameTime)
{
	return false;
}

LRESULT System::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
	{
		Input::getInstance()->keyDown(static_cast<unsigned int>(wParam));
		return 0;
	}
	case WM_KEYUP:
	{
		Input::getInstance()->KeyUp(static_cast<unsigned int>(wParam));
		return 0;
	}
	default:
	{
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	}
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return AppHandle->MessageHandler(hWnd, msg, wParam, lParam);
	}
	}
}
