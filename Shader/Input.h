#pragma once
struct ScreenSize
{
	int Width;
	int Height;
};

class Input
{
private:
	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	Input& operator = (const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator = (Input&&) = delete;

	//入力情報読み取り
	void readKeyBoard();
	void readMouse();
	void readControllers();

	//マウスの移動制限
	void processInput();

	bool initKeyboard();
	bool initMouse();
	bool initControllers();

	//エラーが起きた時に解消する
	void recovery(LPDIRECTINPUTDEVICE8& Dev,HRESULT Hr);

	DIJOYSTATE Padstate_;
	DIJOYSTATE Padprevstate_;
	DIMOUSESTATE Mousesstate_;
	unsigned char Keystate_[256];
	unsigned char Keyprevstate_[256];
	int mousex_, mousey_;
	ScreenSize screen;
	HWND hwnd_;

	//入力情報保存用Vector
	std::vector<ComPtr< IDirectInputDevice8>>gamecontrollers_;
	LPDIRECTINPUTDEVICE8 keyboarddev_;
	LPDIRECTINPUTDEVICE8 mousedevice_;

	//メインデバイス
	ComPtr<IDirectInput8A> didev_;
	unsigned int currentactivecontroller_;
public:



	bool init(HINSTANCE hInstance, HWND Hwnd);
	void update();
	void destroy();

	BOOL EnumGameControllers(LPCDIDEVICEINSTANCE Devinst);

	static BOOL CALLBACK staticEnumGameControllers(LPCDIDEVICEINSTANCE Devinst, LPVOID Pvref);
	static BOOL CALLBACK staticSetGameControllerAxesRange(LPCDIDEVICEINSTANCE Devobjinst, LPVOID Pvref);

	//*******************************
	//		    Keyboard
	//*******************************

	//指定のキーの入力状態を返す
	bool keyDown(const unsigned int KeyCode)const;
	bool KeyUp(const unsigned int KeyCode)const;

	//特定のキーが押されたかを返す
	bool isPressed(const unsigned int KeyCode)const;

	//特定のキーが離されたかを返す
	bool isReleased(const unsigned int KeyCode)const;

	static inline Input* getInstance()
	{
		static Input instance;
		return &instance;
	}

};

