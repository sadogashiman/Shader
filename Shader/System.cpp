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
	wc.lpszClassName = "ClassName";
	wc.cbSize = sizeof(WNDCLASSEX);

	//Window�N���X��o�^
	RegisterClassEx(&wc);

	//�f�X�N�g�b�v��ʉ𑜓x���擾
	RECT rec;
	HWND desktop = GetDesktopWindow();
	GetWindowRect(desktop, &rec);

	ScreenHeight = rec.bottom;
	ScreenWidth = rec.right;

	if (kFullScreen)
	{
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
	else
	{
		//�E�B���h�E���[�h
		ScreenHeight = kWindow_Height;
		ScreenWidth = kWindow_Width;
	}

	//�E�B���h�E�T�C�Y��ݒ�
	const DWORD style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;
	const DWORD ex_style = WS_EX_OVERLAPPEDWINDOW;

	//�덷���C��
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
