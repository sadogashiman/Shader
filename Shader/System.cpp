#include "stdafx.h"
#include "System.h"
#include "Direct3D.h"
#include "Input.h"
#include "ShaderManager.h"
#include"Game.h"
#include"TextureFactory.h"
#include"Timer.h"

//���̂̐錾
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

	//WindowsAPI��������
	initWindows(screenwidth_, screenheight_);

	//DirectX������
	if (!Direct3D::getInstance()->init(screenwidth_, screenheight_, kvsync, kFullScreen, kScreen_depth, kScreen_near))
	{
		Error::showDialog("DirectX�̏������Ɏ��s");
		return false;
	}

	//�C���v�b�g�N���X������
	if (!Input::getInstance()->init())
	{
		Error::showDialog("DirectInput�̏������Ɏ��s");
		return false;
	}

	//�V�F�[�_�[�}�l�[�W���[������
	if (!ShaderManager::getInstance()->init())
	{
		Error::showDialog("�V�F�[�_�[�}�l�[�W���[�̏������Ɏ��s");
		return false;
	}
	//�Q�[���V�X�e��������
	state_.reset(new Game);
	if (!state_.get())
	{
		Error::showDialog("�Q�[���V�X�e���̐����Ɏ��s");
		return false;
	}

	if (!state_.get()->init())
	{
		Error::showDialog("�Q�[���V�X�e���̏������Ɏ��s");
		return false;
	}

	return true;
}

bool System::run()
{
	MSG message;

	//���b�Z�[�W�\���̏�����
	ZeroMemory(&message, sizeof(MSG));

	//���C�����[�v
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

			//60fps�ɌŒ�
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
	//�X�V
	tmp = state_.get()->update();

	//nullptr�Ȃ�I��
	if (tmp == nullptr)
	{
		return false;
	}

	//�V�[���`�F���W
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

	//Lua�t�@�C������E�B���h�E�f�[�^���擾
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	//Lua�t�@�C�����̏������֐��Ăяo��
	if (luaL_dofile(L, "initWindow.lua"))
	{
		Error::showDialog(lua_tostring(L, -1));
		Error::showDialog("�������pLua�t�@�C��������܂���A����̐ݒ�𔽉f���܂�");
		ScreenHeight = kScreenHeight;
		ScreenWidth = kScreenHeight;
		lua_failed = true;
	}

	//lua�t�@�C���̓ǂݍ��݂��������Ă���Ƃ��̂݊֐��̌Ăяo�����s��
	if (!lua_failed)
	{
		//�֐����w��
		lua_getglobal(L, "init");

		//�֐��Ăяo��
		if (lua_pcall(L, 0, 1, 0))
		{
			Error::showDialog(lua_tostring(L, -1));
			throw std::runtime_error("lua�t�@�C������init�֐��̌Ăяo���Ɏ��s���܂���");
		}

		//�e�[�u������p�����[�^�擾
		if (lua_type(L, -1) != LUA_TTABLE)
		{
			throw std::runtime_error("�������e�[�u����init�֐��ŕԂ���Ă��܂���");
		}

		//�X�^�b�N�ɐς�
		lua_getfield(L, 1, "width");
		lua_getfield(L, 1, "height");

		//�E�B���h�E�T�C�Y���e�[�u������擾
		if (lua_type(L, 2) == LUA_TNUMBER)
		{
			ScreenWidth = static_cast<int>(lua_tonumber(L, 2));
		}
		if (lua_type(L, 3) == LUA_TNUMBER)
		{
			ScreenHeight = static_cast<int>(lua_tonumber(L, 3));
		}
	}

	//lua�X�e�[�g��j��
	lua_close(L);

	//���̃I�u�W�F�N�g�ւ̊O���|�C���^���擾
	AppHandle = this;

	//�A�v���P�[�V�����C���X�^���X���擾
	instance_ = GetModuleHandle(NULL);

	//Window�\���̏�����
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	//Window�N���X��ݒ�
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WinProc;
	wc.hInstance = instance_;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = "Lookaway";
	wc.cbSize = sizeof(WNDCLASSEX);

	//Window�N���X��o�^
	RegisterClassEx(&wc);

	//�f�X�N�g�b�v��ʉ𑜓x���擾
	RECT rec;
	HWND desktop = GetDesktopWindow();
	GetWindowRect(desktop, &rec);

	if (kFullScreen)
	{
		ScreenHeight = rec.bottom;
		ScreenWidth = rec.right;

		memset(&dmscreensettings, 0, sizeof(dmscreensettings));

		//�S��ʕ\���ɐݒ�
		dmscreensettings.dmSize = sizeof(dmscreensettings);
		dmscreensettings.dmPelsHeight = static_cast<unsigned long>(ScreenHeight);
		dmscreensettings.dmPelsWidth = static_cast<unsigned long>(ScreenWidth);
		dmscreensettings.dmBitsPerPel = 32;
		dmscreensettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//�S��ʕ\���ɐݒ�
		ChangeDisplaySettings(&dmscreensettings, CDS_FULLSCREEN);
	}

	//�E�B���h�E�T�C�Y��ݒ�
	const DWORD style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
	const DWORD ex_style = WS_EX_OVERLAPPEDWINDOW;

	//�덷���C��
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

	//�E�B���h�E��\��
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
