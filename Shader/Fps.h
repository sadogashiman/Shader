#pragma once
class Fps
{
private:
	std::chrono::high_resolution_clock::time_point starttime_;
	std::chrono::high_resolution_clock::time_point nowtime_;
	int fps_;
	int count_;
public:
	void init();
	void update();
	
	//get
	inline int getFps()const { return fps_; } //現在のフレームレートを返す

};

