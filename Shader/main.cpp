#include"stdafx.h"
#include"System.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
#ifdef _DEBUG
	_CrtSetBreakAlloc(324);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

	//ローカル変数宣言
	std::unique_ptr<System> system;
	bool result;

	//ユニークポインタを初期化
	system.reset(new System);
	if (!system.get())
	{
		return false;
	}

	//システムクラス初期化
	result = system.get()->init();
	if (result)
	{
		//システムクラス実行
		system.get()->run();
	}

	//システムクラス破棄
	system->destroy();

	return 0;
}