#include "stdafx.h"
#include "Timer.h"
#include"Support.h"

bool Timer::init()
{
	//システムが高性能タイマーをサポートしているか確認
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency_);
	if (!frequency_)
	{
		Error::showDialog("システムが高性能タイマーをサポートしていません");
		return false;
	}

	//一ミリ秒の周波数を数える
	ticksperms_ = static_cast<float>(frequency_ / 1000);

	//クロック数を取得
	QueryPerformanceCounter((LARGE_INTEGER*)&starttime_);

	return true;
}

void Timer::update()
{
	UINT64 currenttime;
	float timedifference;

	//現在のクロック数を取得
	QueryPerformanceCounter((LARGE_INTEGER*)&currenttime);

	//前回との差分を計算
	timedifference = static_cast<float>(currenttime - starttime_);

	//このフレームの時間を計算
	frametime_ = timedifference / ticksperms_;

	//現在時刻を開始時刻として保存
	starttime_ = currenttime;
}
