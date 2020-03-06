#pragma once
class Position
{
private:
	Vector3 position_;
	Vector3 rotation_;
	float frametime_;
	float forwardspeed_;
	float backwardspeed_;
	float upwardspeed_;
	float downwardspeed_;
	float leftturnspeed_;
	float rightturnspeed_;
	float lookupspeed_;
	float lookdownspeed_;
public:
	Position();
	//set
	inline void setPosition(Vector3 Position) { position_ = Position; }
	inline void setRotation(Vector3 Rotation) { rotation_ = Rotation; }
	inline void setFrameTime(const float FrameTime) { frametime_ = FrameTime; }

	//get
	Vector3 getPosition() { return position_; }
	Vector3 getRotation() { return rotation_; }

	//move
	void moveForWard(bool KeyDown);
	void moveBackWard(bool KeyDown);
	void moveUpWard(bool KeyDown);
	void moveDownWard(bool KeyDown);

	//turn
	void turnLeft(bool KeyDown);
	void turnRight(bool KeyDown);

	//look
	void lookUpWard(bool KeyDown);
	void lookDownWard(bool KeyDown);

};

