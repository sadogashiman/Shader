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
	
	//�o�^
	finalizers[finalizersizenum++] = Func;
}

void SingletonFinalizer::finalize()
{
	std::lock_guard<std::mutex> lock(mutex);

	//�o�^�����t���ŉ��
	for (int i = finalizersizenum - 1; i >= 0; i--)
	{
		(*finalizers[i])();
	}

	//�v�f����0��
	finalizersizenum = 0;
}
