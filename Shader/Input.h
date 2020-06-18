#pragma once
#include"InputKeycode.h"
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

	DIJOYSTATE padstate_;
	DIJOYSTATE padprevstate_;
	DIMOUSESTATE mousesstate_;
	unsigned char keystate_[256];
	unsigned char keyprevstate_[256];
	int mousex_, mousey_;
	ScreenSize screen_;
	std::chrono::high_resolution_clock::time_point starttime_;

	//���͏��ۑ��pVector
	std::vector<ComPtr< IDirectInputDevice8>>gamecontrollers_;
	LPDIRECTINPUTDEVICE8 keyboarddev_;
	LPDIRECTINPUTDEVICE8 mousedevice_;

	//���C���f�o�C�X
	ComPtr<IDirectInput8A> didev_;
	unsigned int currentactivecontroller_;
public:
	Input(const Input&) = delete;
	Input& operator = (const Input&) = delete;
	Input(Input&&) = delete;
	Input& operator = (Input&&) = delete;


	bool init();
	void update();
	void destroy();

	BOOL EnumGameControllers(LPCDIDEVICEINSTANCE Devinst);

	static BOOL CALLBACK staticEnumGameControllers(LPCDIDEVICEINSTANCE Devinst, LPVOID Pvref);
	static BOOL CALLBACK staticSetGameControllerAxesRange(LPCDIDEVICEINSTANCE Devobjinst, LPVOID Pvref);

	//*******************************
	//		    Keyboard
	//*******************************

	//�w��̃L�[�̓��͏�Ԃ�Ԃ�
	const bool isKeyState(const unsigned int KeyCode)const;

	//����̃L�[�������ꂽ����Ԃ�
	const bool isKeyPressed(const unsigned int KeyCode)const;

	//����̃L�[�������ꂽ����Ԃ�
	const bool isKeyReleased(const unsigned int KeyCode)const;

	//����̃L�[�̓��͂��p������Ă��邩��Ԃ�
	const bool isKeyHeld(const unsigned int KeyCode)const;

	//���炩�̃L�[�����͂���Ă��邩��Ԃ�
	const bool anyKeyDown()const;

	//���炩�̃L�[�̓��͂��I����������Ԃ�
	const bool anyKeyUp()const;

	//���炩�L�[�̓��͂��p������Ă��邩��Ԃ�
	const bool anyKeyHeld()const;


	//*******************************
	//		    GamePad
	//*******************************

	//�w��̃{�^���̓��͏�Ԃ�Ԃ�
	const bool isBottonState(const unsigned int BottonCode)const;
	
	//�w��̃{�^���̓��͂��J�n���ꂽ����Ԃ�
	const bool isBottonPressed(const unsigned int BottonCode)const;
	
	//�w��̃{�^���̓��͂��I�����ꂽ����Ԃ�
	const bool isBottonReleased(const unsigned int BottonCode)const;

	//�w��̃{�^���̓��͂��p������Ă��邩��Ԃ�
	const bool isBottonHeld(const unsigned int BottonCode)const;
	
	//���炩�̃{�^�������͂���Ă��邩��Ԃ�
	const bool anyBottomDown()const;

	//���炩�̃{�^���̓��͂��I�����ꂽ����Ԃ�
	const bool anyBottomUp()const;

	//���炩�̃{�^���̓��͂��p������Ă��邩��Ԃ�
	const bool anyBottonHeld()const;

	//�����POV�̓��͏�Ԃ�Ԃ�
	const bool isPOVState(const unsigned int POVBottonCode)const;

	//�����POV�̓��͂��J�n���ꂽ����Ԃ�
	const bool isPOVPressed(const unsigned int POVBottonCode)const;

	//�����POV�̓��͂��I����������Ԃ�
	const bool isPOVReleased(const unsigned int POVBottonCode)const;

	//�����POV�̓��͂��p������Ă��邩��Ԃ�
	const bool isPOVHeld(const unsigned int POVBottonCode)const;

	//���炩��POV�̓��͂��J�n��������Ԃ�
	const bool anyPadPOVDown()const;

	//���炩��POV�̓��͂��I����������Ԃ�
	const bool anyPadPOVUp()const;

	//���炩��POV�̓��͂��p������Ă��邩�Ԃ�
	const bool anyPadPOVHeld()const;

	//�Q�[���I���L�[
	const bool quitApp();

	static inline Input* getInstance()
	{
		static Input instance;
		return &instance;
	}

};

