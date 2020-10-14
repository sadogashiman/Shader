#include "stdafx.h"
#include "System.h"
#include "Direct3D.h"
#include "Input.h"
#include "ShaderManager.h"
#include"Game.h"
#include"TextureFactory.h"
#include"Timer.h"

//実体の宣言
HINSTANCE System::instance_;
HWND System::hwnd_;
int System::screenheight_;
int System::screenwidth_;

System::System()
{
	ZeroMemory(this, sizeof(System));
}

System::~System()
{
}

bool System::init()
{

	screenheight_ = 0;
	screenwidth_ = 0;

	//WindowsAPIを初期化
	initWindows(screenwidth_, screenheight_);

	//DirectX初期化
	if (!Direct3D::getInstance()->init(screenwidth_, screenheight_, kvsync, kFullScreen, kScreen_depth, kScreen_near))
	{
		Error::showDialog("DirectXの初期化に失敗");
		return false;
	}

	//インプットクラス初期化
	if (!Input::getInstance()->init())
	{
		Error::showDialog("DirectInputの初期化に失敗");
		return false;
	}

	//シェーダーマネージャー初期化
	if (!ShaderManager::getInstance()->init())
	{
		Error::showDialog("シェーダーマネージャーの初期化に失敗");
		return false;
	}
	//ゲームシステム初期化
	state_.reset(new Game);
	if (!state_.get())
	{
		Error::showDialog("ゲームシステムの生成に失敗");
		return false;
	}

	if (!state_.get()->init())
	{
		Error::showDialog("ゲームシステムの初期化に失敗");
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
			Timer::getInstance()->update();

			//60fpsに固定
			if (Timer::getInstance()->fpsControl())
			{
				if (!update())
				{
					return false;
				}

			}
		}
	}
	return true;
}

void System::destroy()
{
	if (state_.get())
		state_.get()->destroy();
	TextureFactory::getInstance()->allDeleteTexture();
	Input::getInstance()->destroy();
	Direct3D::getInstance()->destroy();
	destroyWindows();
}

bool System::update()
{
	State* tmp;
	Input::getInstance()->update();

	if (Input::getInstance()->isKeyPressed(DIK_ESCAPE))
	{
		return false;
	}
	//更新
	tmp = state_.get()->update();

	//nullptrなら終了
	if (tmp == nullptr)
	{
		return false;
	}

	//シーンチェンジ
	if (state_.get() != tmp)
	{
		state_.get()->destroy();
	}

	return true;
}

void System::initWindows(int& ScreenWidth, int& ScreenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmscreensettings;
	bool lua_failed = false;

	//Luaファイルからウィンドウデータを取得
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	//Luaファイル内の初期化関数呼び出し
	if (luaL_dofile(L, "initWindow.lua"))
	{
		Error::showDialog(lua_tostring(L, -1));
		Error::showDialog("初期化用Luaファイルがありません、既定の設定を反映します");
		ScreenHeight = kScreenHeight;
		ScreenWidth = kScreenHeight;
		lua_failed = true;
	}

	//luaファイルの読み込みが成功しているときのみ関数の呼び出しを行う
	if (!lua_failed)
	{
		//関数を指定
		lua_getglobal(L, "init");

		//関数呼び出し
		if (lua_pcall(L, 0, 1, 0))
		{
			Error::showDialog(lua_tostring(L, -1));
			throw std::runtime_error("luaファイルからinit関数の呼び出しに失敗しました");
		}

		//テーブルからパラメータ取得
		if (lua_type(L, -1) != LUA_TTABLE)
		{
			throw std::runtime_error("初期化テーブルがinit関数で返されていません");
		}

		//スタックに積む
		lua_getfield(L, 1, "width");
		lua_getfield(L, 1, "height");

		//ウィンドウサイズをテーブルから取得
		if (lua_type(L, 2) == LUA_TNUMBER)
		{
			ScreenWidth = static_cast<int>(lua_tonumber(L, 2));
		}
		if (lua_type(L, 3) == LUA_TNUMBER)
		{
			ScreenHeight = static_cast<int>(lua_tonumber(L, 3));
		}
	}

	//luaステートを破棄
	lua_close(L);

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
	wc.lpszClassName = "Lookaway";
	wc.cbSize = sizeof(WNDCLASSEX);

	//Windowクラスを登録
	RegisterClassEx(&wc);

	//デスクトップ画面解像度を取得
	RECT rec;
	HWND desktop = GetDesktopWindow();
	GetWindowRect(desktop, &rec);

	if (kFullScreen)
	{
		ScreenHeight = rec.bottom;
		ScreenWidth = rec.right;

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

	//ウィンドウサイズを設定
	const DWORD style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
	const DWORD ex_style = WS_EX_OVERLAPPEDWINDOW;

	//誤差を修正
	AdjustWindowRectEx(&rec, style, false, ex_style);

	hwnd_ = CreateWindowEx(
		ex_style,
		"Lookaway",
		"ShaderApp",
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

LRESULT System::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
	{
		Input::getInstance()->isKeyState(static_cast<unsigned int>(wParam));
		return 0;
	}
	case WM_KEYUP:
	{
		Input::getInstance()->isKeyState(static_cast<unsigned int>(wParam));
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
