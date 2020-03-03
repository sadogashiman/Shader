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
	//稼働状態か確認
	if (running_)
	{
		//現在時間を取得
		nowtime_ = std::chrono::high_resolution_clock::now();

		if (stopwatch_)
		{
			//経過時間を算出
			remaining_ = std::chrono::duration_cast<std::chrono::milliseconds>(nowtime_ - starttime_);
		}
	}
}

void Timer::setTimerStatus(const bool RuningFrag)
{
	//フラグをメンバにコピー
	running_ = RuningFrag;
}

void Timer::startTimer()
{
	//計測開始時間を取得
	starttime_ = std::chrono::high_resolution_clock::now();

	//ストップウォッチ機能を有効にする
	stopwatch_ = true;
}

void Timer::stopTimer()
{
	//計測終了時間を取得
	endtime_ = std::chrono::high_resolution_clock::now();

	//ストップウォッチ機能を無効にする
	stopwatch_ = false;

	//ストップウォッチ機能が有効だった時間を算出
	runningtime_ = std::chrono::duration_cast<std::chrono::milliseconds>(endtime_ - starttime_);

}
