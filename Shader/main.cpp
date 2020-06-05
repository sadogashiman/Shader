#include"stdafx.h"
#include"System.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
#ifdef _DEBUG
	_CrtSetBreakAlloc(324);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

	//���[�J���ϐ��錾
	std::unique_ptr<System> system;
	bool result;

	//���j�[�N�|�C���^��������
	system.reset(new System);
	if (!system.get())
	{
		return false;
	}

	//�V�X�e���N���X������
	result = system.get()->init();
	if (result)
	{
		//�V�X�e���N���X���s
		system.get()->run();
	}

	//�V�X�e���N���X�j��
	system->destroy();

	return 0;
}