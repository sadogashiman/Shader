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

	//���͏��ǂݎ��
	void readKeyBoard();
	void readMouse();
	void readControllers();

	//�}�E�X�̈ړ�����
	void processInput();

	bool initKeyboard();
	bool initMouse();
	bool initControllers();

	//�G���[���N�������ɉ�������
	void recovery(LPDIRECTINPUTDEVICE8& Dev,HRESULT Hr);

	DIJOYSTATE Padstate_;
	DIJOYSTATE Padprevstate_;
	DIMOUSESTATE Mousesstate_;
	unsigned char Keystate_[256];
	unsigned char Keyprevstate_[256];
	int mousex_, mousey_;
	ScreenSize screen;
	HWND hwnd_;

	//���͏��ۑ��pVector
	std::vector<ComPtr< IDirectInputDevice8>>gamecontrollers_;
	LPDIRECTINPUTDEVICE8 keyboarddev_;
	LPDIRECTINPUTDEVICE8 mousedevice_;

	//���C���f�o�C�X
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

	//�w��̃L�[�̓��͏�Ԃ�Ԃ�
	bool keyDown(const unsigned int KeyCode)const;
	bool KeyUp(const unsigned int KeyCode)const;

	//����̃L�[�������ꂽ����Ԃ�
	bool isPressed(const unsigned int KeyCode)const;

	//����̃L�[�������ꂽ����Ԃ�
	bool isReleased(const unsigned int KeyCode)const;

	static inline Input* getInstance()
	{
		static Input instance;
		return &instance;
	}

};

