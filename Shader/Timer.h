#pragma once
class Timer
{
private:
	UINT64 frequency_;
	float ticksperms_;
	UINT64 starttime_;
	float frametime_;
public:
	bool init();
	void update();
	inline const float getTime()const { return frametime_; }	//�t���[���̌o�ߎ��Ԃ�Ԃ�

};

