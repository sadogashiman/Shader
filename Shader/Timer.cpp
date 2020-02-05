#include "stdafx.h"
#include "Timer.h"
#include"Support.h"

bool Timer::init()
{
	//�V�X�e���������\�^�C�}�[���T�|�[�g���Ă��邩�m�F
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency_);
	if (!frequency_)
	{
		Error::showDialog("�V�X�e���������\�^�C�}�[���T�|�[�g���Ă��܂���");
		return false;
	}

	//��~���b�̎��g���𐔂���
	ticksperms_ = static_cast<float>(frequency_ / 1000);

	//�N���b�N�����擾
	QueryPerformanceCounter((LARGE_INTEGER*)&starttime_);

	return true;
}

void Timer::update()
{
	UINT64 currenttime;
	float timedifference;

	//���݂̃N���b�N�����擾
	QueryPerformanceCounter((LARGE_INTEGER*)&currenttime);

	//�O��Ƃ̍������v�Z
	timedifference = static_cast<float>(currenttime - starttime_);

	//���̃t���[���̎��Ԃ��v�Z
	frametime_ = timedifference / ticksperms_;

	//���ݎ������J�n�����Ƃ��ĕۑ�
	starttime_ = currenttime;
}
