#include "stdafx.h"
#include "Input.h"
#include"System.h"

Input::Input()
{
}

Input::~Input()
{
}

bool Input::init()
{
	HRESULT hr;

	//DirectInputインターフェイスを作成
	hr = DirectInput8Create(System::getAppInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)didev_.GetAddressOf(), NULL);

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

	screen_.Height = GetSystemMetrics(SM_CYSCREEN);
	screen_.Width = GetSystemMetrics(SM_CXSCREEN);

	return true;
}

void Input::update()
{
	//前フレームの情報として保存
	padprevstate_ = padstate_;
	memcpy(keyprevstate_, keystate_, sizeof(unsigned char) * 256);

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

const bool Input::isKeyState(const unsigned int KeyCode) const
{
	return keystate_[KeyCode] & DINPUT_VERTION;
}

const bool Input::isKeyPressed(const unsigned int KeyCode) const
{
	return keystate_[KeyCode] & DINPUT_VERTION && !(keyprevstate_[KeyCode] & DINPUT_VERTION);
}

const bool Input::isKeyReleased(const unsigned int KeyCode) const
{
	return !(keystate_[KeyCode] & DINPUT_VERTION) && keyprevstate_[KeyCode] & DINPUT_VERTION;
}

const bool Input::isKeyHeld(const unsigned int KeyCode) const
{
	return keystate_[KeyCode] & DINPUT_VERTION&& keyprevstate_[KeyCode] & DINPUT_VERTION;
}

const bool Input::anyKeyDown() const
{
	for (int i = 0; i < 256; i++)
	{
		if (isKeyPressed(i))
			return true;
	}

	return false;
}

const bool Input::anyKeyUp() const
{
	for (int i = 0; i < 256; i++)
	{
		if (isKeyReleased(i))
			return true;
	}

	return false;
}

const bool Input::anyKeyHeld() const
{
	for (int i = 0; i < 256; i++)
	{
		if (isKeyHeld(i))
			return true;
	}

	return false;
}

const bool Input::isBottonState(const unsigned int BottonCode) const
{
	return padstate_.rgbButtons[BottonCode] & DINPUT_VERTION;
}

const bool Input::isBottonPressed(const unsigned int BottonCode) const
{
	return padstate_.rgbButtons[BottonCode] & DINPUT_VERTION && !(padprevstate_.rgbButtons[BottonCode] & DINPUT_VERTION);
}

const bool Input::isBottonReleased(const unsigned int BottonCode) const
{
	return !(padstate_.rgbButtons[BottonCode] & DINPUT_VERTION) && padprevstate_.rgbButtons[BottonCode] & DINPUT_VERTION;
}

const bool Input::isBottonHeld(const unsigned int BottonCode) const
{
	return padstate_.rgbButtons[BottonCode] & DINPUT_VERTION && padprevstate_.rgbButtons[BottonCode] & DINPUT_VERTION;
}

const bool Input::anyBottomDown() const
{
	for (int i = 0; i < 32; i++)
	{
		if (isBottonPressed(i))
			return true;
	}

	return false;
}

const bool Input::anyBottomUp() const
{
	for (int i = 0; i < 32; i++)
	{
		if (isBottonReleased(i))
			return true;
	}

	return false;
}

const bool Input::anyBottonHeld() const
{
	for (int i = 0; i < 32; i++)
	{
		if (isBottonHeld(i))
			return true;
	}

	return false;
}

const bool Input::isPOVState(const unsigned int POVBottonCode) const
{
	return padstate_.rgdwPOV[0] == 9000 * POVBottonCode;
}

const bool Input::isPOVPressed(const unsigned int POVBottonCode) const
{
	return padstate_.rgdwPOV[0] == 9000 * POVBottonCode && padprevstate_.rgdwPOV[0] != 9000 * POVBottonCode;
}

const bool Input::isPOVReleased(const unsigned int POVBottonCode) const
{
	return padstate_.rgdwPOV[0] != 9000 * POVBottonCode && padprevstate_.rgdwPOV[0] == 9000 * POVBottonCode;
}

const bool Input::isPOVHeld(const unsigned int POVBottonCode) const
{
	return padstate_.rgdwPOV[0] == 9000 * POVBottonCode && padprevstate_.rgdwPOV[0] == 9000 * POVBottonCode;
}

const bool Input::anyPadPOVDown() const
{
	for (int i = 0; i < 4; i++)
	{
		if (isPOVPressed(i))
			return true;
	}

	return false;
}

const bool Input::anyPadPOVUp() const
{
	for (int i = 0; i < 4; i++)
	{
		if (isPOVReleased(i))
			return true;
	}

	return false;
}

const bool Input::anyPadPOVHeld() const
{
	for (int i = 0; i < 4; i++)
	{
		if (isPOVHeld(i))
			return true;
	}

	return false;
}

const bool Input::quitApp()
{
	if (kTgs)
	{
		if (isBottonPressed(DINPUT_BOTTON_8))
			starttime_ = std::chrono::high_resolution_clock::now();
		return true;
	}
	else
	{
		//ESCが押されたらアプリケーションを終了
		if (isKeyPressed(DIK_ESCAPE))
		{
			return true;
		}
	}

	return false;
}

void Input::readKeyBoard()
{
	HRESULT hr;
	hr = keyboarddev_->GetDeviceState(sizeof(keystate_), &keystate_);
	recovery(keyboarddev_, hr);
}

void Input::readMouse()
{
	mousedevice_->GetDeviceState(sizeof(padstate_), &padstate_);
}

void Input::readControllers()
{
	//すべてのコントローラーデバイスを稼働状態に
	for (auto controller : gamecontrollers_)
	{
		//前回の情報として記録
		padprevstate_ = padstate_;

		controller->GetDeviceState(sizeof(controller), controller.GetAddressOf());
	}
}

void Input::processInput()
{
	mousex_ = mousesstate_.lX;
	mousey_ = mousesstate_.lY;

	//移動制限
	if (mousex_ < 0)
		mousex_ = 0;

	if (mousey_ < 0)
		mousey_ = 0;

	if (mousex_ > screen_.Width)
		mousex_ = screen_.Width;

	if (mousey_ > screen_.Height)
		mousey_ = screen_.Height;
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
	if (FAILED(keyboarddev_->SetCooperativeLevel(System::getWindowHandle(), DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
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
	if (FAILED(mousedevice_->SetCooperativeLevel(System::getWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)))
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
	if (FAILED(gamecontroller->SetCooperativeLevel(System::getWindowHandle(), DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
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

