#include "stdafx.h"
#include "Timer.h"
#include "Support.h"

Timer::Timer()
{
	ZeroMemory(this, sizeof(Timer));
	running_ = false;
	stopwatch_ = false;
	prevtime_ = std::chrono::high_resolution_clock::now();
}

Timer::~Timer()
{
}

bool Timer::init()
{
	//�V�X�e�����^�C�}�[���T�|�[�g���Ă��邩�m�F
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency_);
	if (frequency_ == 0)
	{
		Error::showDialog("�V�X�e�����^�C�}�[���T�|�[�g���Ă��܂���");
		return false;
	}

	//�~���b������̃N���b�N�����Z�o
	tickperms_ = static_cast<float>(frequency_ / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&cpustart_);


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

	INT64 currenttime;
	float timediffrence;
	QueryPerformanceCounter((LARGE_INTEGER*)&currenttime);
	timediffrence = static_cast<float>(currenttime - cpustart_);
	frametime_ = timediffrence / tickperms_;
	cpustart_ = currenttime;
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

bool Timer::fpsControl(FrameRate Rate)
{
	//�O��Ƃ̍������v�Z
	auto difference = std::chrono::duration_cast<std::chrono::microseconds>((nowtime_ - prevtime_));
	//16�~���b�o�߂��Ă�����X�V��������
	if (difference.count() > Rate)
	{
		prevtime_ = nowtime_;
		return true;
	}

	return false;
}
