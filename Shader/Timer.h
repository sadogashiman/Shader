#pragma once
class Timer
{
private:
	bool running_;												 //�ғ���Ԃ��𔻒肷��t���O
	bool stopwatch_;											 //���Ԍv�������邩�𔻒肷��t���O
	std::chrono::high_resolution_clock::time_point starttime_;   //�v���J�n����
	std::chrono::high_resolution_clock::time_point nowtime_;     //���ݎ���
	std::chrono::high_resolution_clock::time_point endtime_;	 //�v���I������
	std::chrono::milliseconds runningtime_;						 //�^�C�}�[���ғ����Ă�������
	std::chrono::milliseconds remaining_;						 //�O��v�����Ƃ̍���
	
public:
	Timer();
	~Timer();
	void update();								//�^�C�}�[�̍X�V������֐�(�^�C�}�[���ғ���ԂłȂ��Ƃ��͉������Ȃ�)
	void setTimerStatus(const bool RuningFrag); //�^�C�}�[�̍X�V�����邩�ǂ�����ݒ肷��֐�
	void startTimer();							//���Ԍv�����J�n����֐�
	void stopTimer();							//���Ԍv�����I������֐�
	
	//get
	inline const UINT64 getElapsedTime()const { return remaining_.count(); }		//�v���J�n����֐��Ăяo�����܂ł̌o�ߎ��Ԃ�Ԃ�(�~���b)
	inline const UINT64 getRunningTime()const { return runningtime_.count(); }		//�v���J�n����I���܂ł̌o�ߎ��Ԃ�Ԃ�(�~���b)
	inline const bool getTimerStatus()const { return running_; }				//�^�C�}�[�����݉ғ����Ă��邩�ǂ�����Ԃ�
	inline const bool getStopWatchStatus()const { return stopwatch_; }			//�X�g�b�v�E�H�b�`�@�\���L�����ǂ�����Ԃ��֐�
};

