#include "stdafx.h"
#include "Input.h"

Input::Input()
{
}

Input::~Input()
{
}

bool Input::init(HINSTANCE hInstance, HWND Hwnd)
{
	//�E�B�h�E�n���h����ۑ�
	hwnd_ = Hwnd;
	HRESULT hr;

	//DirectInput�C���^�[�t�F�C�X���쐬
	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)didev_.GetAddressOf(), NULL);

	//���̓f�o�C�X�̗�
	//hr = didev_.Get()->EnumDevices(DI8DEVCLASS_GAMECTRL, &staticEnumGameControllers, this, DIEDFL_ATTACHEDONLY);
	//if (FAILED(hr))
	//{
	//	return false;
	//}

	if (FAILED(hr))
	{
		return false;
	}

	//�e�f�o�C�X��������
	if (!initKeyboard())
	{
		return false;
	}

	if (!initMouse())
	{
		return false;
	}

	/*if (!initControllers())
	{
		return false;
	}*/

	screen.Height = GetSystemMetrics(SM_CYSCREEN);
	screen.Width = GetSystemMetrics(SM_CXSCREEN);

	return true;
}

void Input::update()
{
	//�O�t���[���̏��Ƃ��ĕۑ�
	Padprevstate_ = Padstate_;
	memcpy(Keyprevstate_, Keyprevstate_, sizeof(unsigned char));

	//�e�f�o�C�X�̓��͏����擾
	//readControllers();
	readKeyBoard();
	readMouse();
	processInput();
}

void Input::destroy()
{
	//�j��
	//for (auto& controller : gamecontrollers_)
	//{
	//	controller.Get()->Unacquire();
	//}

	mousedevice_->Unacquire();
	mousedevice_->Release();

	keyboarddev_->Unacquire();
	keyboarddev_->Release();
}

BOOL Input::EnumGameControllers(LPCDIDEVICEINSTANCE Devinst)
{
	LPDIRECTINPUTDEVICE8 gamecontroller;

	//���݂̃R���g���[���[�̃C���^�[�t�F�C�X���쐬
	if (FAILED(didev_->CreateDevice(Devinst->guidInstance, &gamecontroller, NULL)))
	{
		return DIENUM_CONTINUE;
	}
	else
	{
		//�R���g���[���[��ۑ�
		gamecontrollers_.push_back(gamecontroller);
		return DIENUM_CONTINUE;
	}
}

BOOL Input::staticEnumGameControllers(LPCDIDEVICEINSTANCE Devinst, LPVOID Pvref)
{
	Input* dinputinstance = (Input*)Pvref;
	return dinputinstance->EnumGameControllers(Devinst);
}

BOOL Input::staticSetGameControllerAxesRange(LPCDIDEVICEINSTANCE Devobjinst, LPVOID Pvref)
{
	//�R���g���[���[
	LPDIRECTINPUTDEVICE8 gamecontroller = (LPDIRECTINPUTDEVICE8)Pvref;
	gamecontroller->Unacquire();

	//�R���g���[���[�̃v���p�e�B��ێ�����p
	DIPROPRANGE gamecontrollerrange;

	//�͈͂�ݒ�
	gamecontrollerrange.lMax = -100;
	gamecontrollerrange.lMin = 100;

	//�T�C�Y��ݒ�
	gamecontrollerrange.diph.dwSize = sizeof(DIPROPRANGE);
	gamecontrollerrange.diph.dwHeaderSize = sizeof(DIPROPRANGE);

	//�ύX����I�u�W�F�N�g��ݒ�
	gamecontrollerrange.diph.dwHow = DIPH_BYID;
	gamecontrollerrange.diph.dwObj = Devobjinst->dwDevType;

	//���͈̔͂�ݒ�
	if (FAILED(gamecontroller->SetProperty(DIPROP_RANGE, &gamecontrollerrange.diph)))
	{
		return DIENUM_STOP;
	}

	//���̃f�b�g�]�[����ێ�����p
	DIPROPDWORD gamecontrollerdeadzone;

	//�f�b�h�]�[����1%�ɐݒ�
	gamecontrollerdeadzone.diph.dwSize = sizeof(DIPROPDWORD);
	gamecontrollerdeadzone.diph.dwHeaderSize = sizeof(DIPROPHEADER);

	//�ύX����I�u�W�F�N�g��ݒ�
	gamecontrollerdeadzone.diph.dwHow = DIPH_BYID;
	gamecontrollerdeadzone.diph.dwObj = Devobjinst->dwDevType;

	//���̃f�b�h�]�[���̐ݒ�
	if (FAILED(gamecontroller->SetProperty(DIPROP_DEADZONE, &gamecontrollerdeadzone.diph)))
	{
		return DIENUM_STOP;
	}

	if (FAILED(gamecontroller->SetProperty(DIPROP_DEADZONE, &gamecontrollerdeadzone.diph)))
	{
		return false;
	}

	return DIENUM_CONTINUE;
}

bool Input::keyDown(const unsigned int KeyCode) const
{
	return Keystate_[KeyCode] & DINPUT_VERTION;
}

bool Input::KeyUp(const unsigned int KeyCode) const
{
	return !(Keystate_[KeyCode] & DINPUT_VERTION);
}

bool Input::isPressed(const unsigned int KeyCode) const
{
	return Keystate_[KeyCode] & DINPUT_VERTION && !(Keyprevstate_[KeyCode] & DINPUT_VERTION);
}

bool Input::isReleased(const unsigned int KeyCode) const
{
	return !(Keystate_[KeyCode] & DINPUT_VERTION) && Keyprevstate_[KeyCode] & DINPUT_VERTION;
}

const bool Input::anyKeyDown(const unsigned int KeyCode) const
{
	return false;
}

const bool Input::anyKeyUp(const unsigned int KeyCode) const
{
	return false;
}

const bool Input::quitApp() const
{
	return false;
}

void Input::readKeyBoard()
{
	HRESULT hr;
	hr = keyboarddev_->GetDeviceState(sizeof(Keystate_), &Keystate_);
	recovery(keyboarddev_, hr);
}

void Input::readMouse()
{
	mousedevice_->GetDeviceState(sizeof(Padstate_), &Padstate_);
}

void Input::readControllers()
{
	//���ׂẴR���g���[���[�f�o�C�X���ғ���Ԃ�
	for (auto controller : gamecontrollers_)
	{
		//�O��̏��Ƃ��ċL�^
		Padprevstate_ = Padstate_;

		controller->GetDeviceState(sizeof(controller), controller.GetAddressOf());
	}
}

void Input::processInput()
{
	mousex_ = Mousesstate_.lX;
	mousey_ = Mousesstate_.lY;

	//�ړ�����
	if (mousex_ < 0)
		mousex_ = 0;

	if (mousey_ < 0)
		mousey_ = 0;

	if (mousex_ > screen.Width)
		mousex_ = screen.Width;

	if (mousey_ > screen.Height)
		mousey_ = screen.Height;
}

bool Input::initKeyboard()
{
	//�L�[�{�[�h�C���^�[�t�F�C�X��������
	if (FAILED(didev_->CreateDevice(GUID_SysKeyboard, &keyboarddev_, NULL)))
	{
		return false;
	}

	//�f�[�^�t�H�[�}�b�g�쐬
	if (FAILED(keyboarddev_->SetDataFormat(&c_dfDIKeyboard)))
	{
		return false;
	}

	//�������x���̐ݒ�
	if (FAILED(keyboarddev_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
	{
		return false;
	}

	//�ғ���Ԃ�
	keyboarddev_->Acquire();

	return true;
}

bool Input::initMouse()
{
	//�f�o�C�X�쐬
	if (FAILED(didev_->CreateDevice(GUID_SysMouse, &mousedevice_, NULL)))
	{
		return false;
	}

	//�f�[�^�t�H�[�}�b�g�̐ݒ�
	if (FAILED(mousedevice_->SetDataFormat(&c_dfDIMouse)))
	{
		return false;
	}

	//�������x���̐ݒ�(�}�E�X��Windnows�̋@�\���g���Ƃ��ɕK�v�Ȃ̂Ő�L���Ȃ�)
	if (FAILED(mousedevice_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		return false;
	}

	//�G���[���N�������������
	HRESULT hr = mousedevice_->Acquire();
	recovery(mousedevice_, hr);

	return true;
}

bool Input::initControllers()
{
	if (currentactivecontroller_ < 0 || currentactivecontroller_ >= gamecontrollers_.size())
	{
		return false;
	}

	//���݃A�N�e�B�u�ȃR���g���[���[���擾
	LPDIRECTINPUTDEVICE8 gamecontroller = gamecontrollers_[currentactivecontroller_].Get();

	//�������x���̐ݒ�
	if (FAILED(gamecontroller->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
	{
		return false;
	}

	//�f�[�^�t�H�[�}�b�g�̐ݒ�
	if (FAILED(gamecontroller->SetDataFormat(&c_dfDIJoystick)))
	{
		return false;
	}

	HRESULT hr = gamecontroller->Acquire();
	if (FAILED(hr))
	{
		//�G���[���N�������������
		recovery(gamecontroller, hr);
	}

	return true;
}

void Input::recovery(LPDIRECTINPUTDEVICE8& Dev, HRESULT Hr)
{
	//�G���[���Ȃ��Ȃ�܂Ń��J�o���[
	while (Hr == DIERR_INPUTLOST || Hr == DIERR_INVALIDPARAM || Hr == DIERR_NOTACQUIRED || Hr == DIERR_NOTINITIALIZED || Hr == E_PENDING)
	{
		Hr = Dev->Acquire();
	}
}

