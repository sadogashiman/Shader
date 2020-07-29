#include"stdafx.h"
#include"System.h"
#include"Singleton.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
#ifdef _DEBUG
	//_CrtSetBreakAlloc(1052222);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

	//�V�X�e���N���X���V���O���g���œo�^
	System* system = Singleton<System>::getInstancePtr();

	//������
	if (system->init())
	{
		//����
		system->run();
	}

	//�o�^�I�u�W�F�N�g�̔j���E���
	SingletonFinalizer::finalize();

	return 0;
}