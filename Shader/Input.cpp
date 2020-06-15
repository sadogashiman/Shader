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
	//ウィドウハンドルを保存
	hwnd_ = Hwnd;
	HRESULT hr;

	//DirectInputインターフェイスを作成
	hr = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)didev_.GetAddressOf(), NULL);

	//入力デバイスの列挙
	//hr = didev_.Get()->EnumDevices(DI8DEVCLASS_GAMECTRL, &staticEnumGameControllers, this, DIEDFL_ATTACHEDONLY);
	//if (FAILED(hr))
	//{
	//	return false;
	//}

	if (FAILED(hr))
	{
		return false;
	}

	//各デバイスを初期化
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
	//前フレームの情報として保存
	Padprevstate_ = Padstate_;
	memcpy(Keyprevstate_, Keyprevstate_, sizeof(unsigned char));

	//各デバイスの入力情報を取得
	//readControllers();
	readKeyBoard();
	readMouse();
	processInput();
}

void Input::destroy()
{
	//破棄
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

	//現在のコントローラーのインターフェイスを作成
	if (FAILED(didev_->CreateDevice(Devinst->guidInstance, &gamecontroller, NULL)))
	{
		return DIENUM_CONTINUE;
	}
	else
	{
		//コントローラーを保存
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
	//コントローラー
	LPDIRECTINPUTDEVICE8 gamecontroller = (LPDIRECTINPUTDEVICE8)Pvref;
	gamecontroller->Unacquire();

	//コントローラーのプロパティを保持する用
	DIPROPRANGE gamecontrollerrange;

	//範囲を設定
	gamecontrollerrange.lMax = -100;
	gamecontrollerrange.lMin = 100;

	//サイズを設定
	gamecontrollerrange.diph.dwSize = sizeof(DIPROPRANGE);
	gamecontrollerrange.diph.dwHeaderSize = sizeof(DIPROPRANGE);

	//変更するオブジェクトを設定
	gamecontrollerrange.diph.dwHow = DIPH_BYID;
	gamecontrollerrange.diph.dwObj = Devobjinst->dwDevType;

	//軸の範囲を設定
	if (FAILED(gamecontroller->SetProperty(DIPROP_RANGE, &gamecontrollerrange.diph)))
	{
		return DIENUM_STOP;
	}

	//軸のデットゾーンを保持する用
	DIPROPDWORD gamecontrollerdeadzone;

	//デッドゾーンを1%に設定
	gamecontrollerdeadzone.diph.dwSize = sizeof(DIPROPDWORD);
	gamecontrollerdeadzone.diph.dwHeaderSize = sizeof(DIPROPHEADER);

	//変更するオブジェクトを設定
	gamecontrollerdeadzone.diph.dwHow = DIPH_BYID;
	gamecontrollerdeadzone.diph.dwObj = Devobjinst->dwDevType;

	//軸のデッドゾーンの設定
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
	//すべてのコントローラーデバイスを稼働状態に
	for (auto controller : gamecontrollers_)
	{
		//前回の情報として記録
		Padprevstate_ = Padstate_;

		controller->GetDeviceState(sizeof(controller), controller.GetAddressOf());
	}
}

void Input::processInput()
{
	mousex_ = Mousesstate_.lX;
	mousey_ = Mousesstate_.lY;

	//移動制限
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
	//キーボードインターフェイスを初期化
	if (FAILED(didev_->CreateDevice(GUID_SysKeyboard, &keyboarddev_, NULL)))
	{
		return false;
	}

	//データフォーマット作成
	if (FAILED(keyboarddev_->SetDataFormat(&c_dfDIKeyboard)))
	{
		return false;
	}

	//協調レベルの設定
	if (FAILED(keyboarddev_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
	{
		return false;
	}

	//稼働状態に
	keyboarddev_->Acquire();

	return true;
}

bool Input::initMouse()
{
	//デバイス作成
	if (FAILED(didev_->CreateDevice(GUID_SysMouse, &mousedevice_, NULL)))
	{
		return false;
	}

	//データフォーマットの設定
	if (FAILED(mousedevice_->SetDataFormat(&c_dfDIMouse)))
	{
		return false;
	}

	//協調レベルの設定(マウスはWindnowsの機能を使うときに必要なので占有しない)
	if (FAILED(mousedevice_->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
	{
		return false;
	}

	//エラーが起きたら解消する
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

	//現在アクティブなコントローラーを取得
	LPDIRECTINPUTDEVICE8 gamecontroller = gamecontrollers_[currentactivecontroller_].Get();

	//協調レベルの設定
	if (FAILED(gamecontroller->SetCooperativeLevel(hwnd_, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
	{
		return false;
	}

	//データフォーマットの設定
	if (FAILED(gamecontroller->SetDataFormat(&c_dfDIJoystick)))
	{
		return false;
	}

	HRESULT hr = gamecontroller->Acquire();
	if (FAILED(hr))
	{
		//エラーが起きたら解消する
		recovery(gamecontroller, hr);
	}

	return true;
}

void Input::recovery(LPDIRECTINPUTDEVICE8& Dev, HRESULT Hr)
{
	//エラーがなくなるまでリカバリー
	while (Hr == DIERR_INPUTLOST || Hr == DIERR_INVALIDPARAM || Hr == DIERR_NOTACQUIRED || Hr == DIERR_NOTINITIALIZED || Hr == E_PENDING)
	{
		Hr = Dev->Acquire();
	}
}

