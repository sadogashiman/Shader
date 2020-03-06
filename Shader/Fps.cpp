#include "stdafx.h"
#include "Fps.h"

void Fps::init()
{
	fps_ = 0;
	count_ = 0;

	//現在の時間を取得
	starttime_ = std::chrono::high_resolution_clock::now();

}

void Fps::update()
{
	//フレームレートカウントを加算
	count_++;

	//現在の時間を取得
	nowtime_ = std::chrono::high_resolution_clock::now();

	//差分を計算
	auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(nowtime_ - starttime_);

	//一秒間経っていたらフレームレートを代入し現在時間を前回のものとして記録
	if (tmp.count() > 1000)
	{
		fps_ = count_;
		count_ = 0;

		//現在時間を開始時間に設定
		starttime_ = nowtime_;

	}
}
