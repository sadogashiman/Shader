#pragma once
const int kQuitBottom = DIK_ESCAPE; //アプリケーション終了ボタン
const bool kTgs = false; //TGSブース展示用ボタン配置

struct ScreenSize
{
	int Width;
	int Height;
};

class Input
{
private:
	Input();
	~Input();

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

	DIJOYSTATE padstate_;
	DIJOYSTATE padprevstate_;
	DIMOUSESTATE Mousesstate_;
	unsigned char Keystate_[256];
	unsigned char keyprevstate_[256];
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
	Input(const Input&) = delete;
	Input& operator = (const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator = (Input&&) = delete;


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

	const bool anyKeyDown(const unsigned int KeyCode)const;
	const bool anyKeyUp(const unsigned int KeyCode)const;
	const bool quitApp()const;

};

