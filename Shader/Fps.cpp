#include "stdafx.h"
#include "Fps.h"

void Fps::init()
{
	fps_ = 0;
	count_ = 0;

	//���݂̎��Ԃ��擾
	starttime_ = std::chrono::high_resolution_clock::now();

}

void Fps::update()
{
	//�t���[�����[�g�J�E���g�����Z
	count_++;

	//���݂̎��Ԃ��擾
	nowtime_ = std::chrono::high_resolution_clock::now();

	//�������v�Z
	auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(nowtime_ - starttime_);

	//��b�Ԍo���Ă�����t���[�����[�g���������ݎ��Ԃ�O��̂��̂Ƃ��ċL�^
	if (tmp.count() > 1000)
	{
		fps_ = count_;
		count_ = 0;

		//���ݎ��Ԃ��J�n���Ԃɐݒ�
		starttime_ = nowtime_;

	}
}
