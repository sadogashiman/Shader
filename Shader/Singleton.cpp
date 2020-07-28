#include "stdafx.h"
#include "Singleton.h"
namespace
{
	constexpr int kMaxFinalizerSize = 256;
	std::mutex mutex;
	int finalizersizenum;
	SingletonFinalizer::finalizerFunc finalizers[kMaxFinalizerSize];
}

void SingletonFinalizer::addFinalizer(finalizerFunc Func)
{
	std::lock_guard<std::mutex>lock(mutex);
	
	//ìoò^
	finalizers[finalizersizenum++] = Func;
}

void SingletonFinalizer::finalize()
{
	std::lock_guard<std::mutex> lock(mutex);

	//ìoò^ÇµÇΩãtèáÇ≈âï˙
	for (int i = finalizersizenum - 1; i >= 0; i--)
	{
		(*finalizers[i])();
	}

	//óvëfêîÇ0Ç…
	finalizersizenum = 0;
}
