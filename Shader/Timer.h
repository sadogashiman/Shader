#pragma once
enum FrameRate
{
	k60Hz = 16666,
	k144Hz = 6944,
	k244Hz = 4098,
};

class Timer
{
private:
	Timer();
	~Timer();
	Timer(const Timer&) = delete;
	Timer& operator=(const Timer&) = delete;
	Timer(Timer&&) = delete;
	Timer& operator=(Timer&&) = delete;

	bool running_;												 //�ғ���Ԃ��𔻒肷��t���O
	bool stopwatch_;											 //���Ԍv�������邩�𔻒肷��t���O
	std::chrono::high_resolution_clock::time_point starttime_;   //�v���J�n����
	std::chrono::high_resolution_clock::time_point prevtime_;    //�O�񎞊Ԃ�}�������̎���
	std::chrono::high_resolution_clock::time_point nowtime_;     //���ݎ���
	std::chrono::high_resolution_clock::time_point endtime_;	 //�v���I������
	std::chrono::milliseconds runningtime_;						 //�^�C�}�[���ғ����Ă�������
	std::chrono::milliseconds remaining_;						 //�O��v�����Ƃ̍���
	std::chrono::milliseconds startuptime_;

	//cpu�N���b�N
	float tickperms_;
	float frametime_;
	INT64 frequency_;
	INT64 cpustart_;

public:
	bool init();
	void update();								//�^�C�}�[�̍X�V������֐�(�^�C�}�[���ғ���ԂłȂ��Ƃ��͉������Ȃ�)
	void setTimerStatus(const bool RuningFrag); //�^�C�}�[�̍X�V�����邩�ǂ�����ݒ肷��֐�
	void startTimer();							//���Ԍv�����J�n����֐�
	void stopTimer();							//���Ԍv�����I������֐�
	bool fpsControl(FrameRate Rate = k60Hz);	//�t���[���X�V������^�C�~���O��true��Ԃ��֐�

	//get
	inline const UINT64 getElapsedTime()const { return remaining_.count(); }		//�v���J�n����֐��Ăяo�����܂ł̌o�ߎ��Ԃ�Ԃ�(�~���b)
	inline const UINT64 getRunningTime()const { return runningtime_.count(); }		//�v���J�n����I���܂ł̌o�ߎ��Ԃ�Ԃ�(�~���b)
	inline const bool getTimerStatus()const { return running_; }					//�^�C�}�[�����݉ғ����Ă��邩�ǂ�����Ԃ�
	inline const bool getStopWatchStatus()const { return stopwatch_; }				//�X�g�b�v�E�H�b�`�@�\���L�����ǂ�����Ԃ��֐�
	inline const float getCpuTime()const { return frametime_; }
	static inline Timer* getInstance() //�C���X�^���X��Ԃ�
	{
		static Timer instance_;
		return &instance_;
	}

};

