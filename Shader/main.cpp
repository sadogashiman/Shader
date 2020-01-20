#include"stdafx.h"
#include"System.h"
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
#ifdef _DEBUG
	//_CrtSetBreakAlloc(596);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

	System* system;

	bool result;

	system = new System;
	if (!system)
	{
		return false;
	}

	result = system->init();
	if (result)
	{
		system->run();
	}

	system->destroy();
	delete system;
	system = nullptr;

	return 0;
}
