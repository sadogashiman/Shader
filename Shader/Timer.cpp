#include "stdafx.h"
#include "Timer.h"
#include "Support.h"

Timer::Timer()
{
	running_ = false;
	stopwatch_ = false;
}

Timer::~Timer()
{
}

void Timer::update()
{
	//�ғ���Ԃ��m�F
	if (running_)
	{
		//���ݎ��Ԃ��擾
		nowtime_ = std::chrono::high_resolution_clock::now();

		if (stopwatch_)
		{
			//�o�ߎ��Ԃ��Z�o
			remaining_ = std::chrono::duration_cast<std::chrono::milliseconds>(nowtime_ - starttime_);
		}
	}
}

void Timer::setTimerStatus(const bool RuningFrag)
{
	//�t���O�������o�ɃR�s�[
	running_ = RuningFrag;
}

void Timer::startTimer()
{
	//�v���J�n���Ԃ��擾
	starttime_ = std::chrono::high_resolution_clock::now();

	//�X�g�b�v�E�H�b�`�@�\��L���ɂ���
	stopwatch_ = true;
}

void Timer::stopTimer()
{
	//�v���I�����Ԃ��擾
	endtime_ = std::chrono::high_resolution_clock::now();

	//�X�g�b�v�E�H�b�`�@�\�𖳌��ɂ���
	stopwatch_ = false;

	//�X�g�b�v�E�H�b�`�@�\���L�����������Ԃ��Z�o
	runningtime_ = std::chrono::duration_cast<std::chrono::milliseconds>(endtime_ - starttime_);

}
