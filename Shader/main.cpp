#include"stdafx.h"
#include"System.h"
#include"Singleton.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
#ifdef _DEBUG
	//_CrtSetBreakAlloc(1052222);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

	//システムクラスをシングルトンで登録
	System* system = Singleton<System>::getInstancePtr();

	//初期化
	if (system->init())
	{
		//処理
		system->run();
	}

	//登録オブジェクトの破棄・解放
	SingletonFinalizer::finalize();

	return 0;
}