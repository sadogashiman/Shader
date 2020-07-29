#include "stdafx.h"
#include "Singleton.h"
namespace
{
	std::mutex mutex;
	std::vector<SingletonFinalizer::finalizerFunc> finalizers;
}

void SingletonFinalizer::addFinalizer(finalizerFunc Func)
{
	std::lock_guard<std::mutex>lock(mutex);
	
	//�o�^
	finalizers.push_back(Func);
}

void SingletonFinalizer::finalize()
{
	std::lock_guard<std::mutex> lock(mutex);

	//�z��T�C�Y�擾
	unsigned int size = finalizers.size();

	//�o�^�����t���ŉ��
	for (int i = size - 1; i >= 0; i--)
	{
		(*finalizers[i])();
	}

	//�v�f���N���A
	finalizers.clear();
}
