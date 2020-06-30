#pragma once
class Position
{
private:
	Vector3 position_;
	Vector3 rotation_;
	float frametime_;
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
	void moveLeftWard(bool KeyDown);
	void moveRightWard(bool KeyDown);
		
	//turn
	void turnLeft(bool KeyDown);
	void turnRight(bool KeyDown);

	//look
	void lookUpWard(bool KeyDown);
	void lookDownWard(bool KeyDown);

};

