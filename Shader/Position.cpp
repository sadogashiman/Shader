#include "stdafx.h"
#include "Position.h"
const float kMovespeed = 1.0F;
const float kTurnspeed = 0.5F;
const float kLookspeed = 0.3F;

Position::Position()
{
	ZeroMemory(this, sizeof(Position));
	position_.y = 2.0F;
	position_.x = 50.0F;
}

void Position::moveForWard(bool KeyDown)
{
	float radian;

	//キーが押されている間加速
	if (KeyDown)
	{
		forwardspeed_ = kMovespeed;
	}
	else
	{
		forwardspeed_ = 0.0F;
	}

	//ラジアンに変換して代入
	radian = XMConvertToRadians(rotation_.y);

	position_.x += sin(radian) * forwardspeed_;
	position_.z += cos(radian) * forwardspeed_;
}

void Position::moveBackWard(bool KeyDown)
{
	float radian;

	//キーが押されている間加速
	if (KeyDown)
	{
		backwardspeed_ = kMovespeed;
	}
	else
	{
		backwardspeed_ = 0.0F;
	}

	//ラジアンに変換して代入
	radian = XMConvertToRadians(rotation_.y);

	position_.x -= sin(radian) * backwardspeed_;
	position_.z -= cos(radian) * backwardspeed_;
}

void Position::moveUpWard(bool KeyDown)
{
	//キーが押されている間加速
	if (KeyDown)
	{
		upwardspeed_ = kMovespeed;
	}
	else
	{
		upwardspeed_ = 0.0F;
	}

	position_.y += upwardspeed_;
}

void Position::moveDownWard(bool KeyDown)
{
	//キーが押されている間加速
	if (KeyDown)
	{
		downwardspeed_ = kMovespeed;
	}
	else
	{
		downwardspeed_ = 0.0F;

	}

	position_.y -= downwardspeed_;
}

void Position::turnLeft(bool KeyDown)
{
	//キーが押されている間加速
	if (KeyDown)
	{
		leftturnspeed_ = kTurnspeed;
	}
	else
	{
		leftturnspeed_ = 0.0F;

	}

	rotation_.y -= leftturnspeed_;

	if (rotation_.y < 0.0F)
	{
		rotation_.y += 360.0F;
	}
}

void Position::turnRight(bool KeyDown)
{
	//キーが押されている間加速
	if (KeyDown)
	{
		rightturnspeed_ = kTurnspeed;
	}
	else
	{
		rightturnspeed_ = 0.0F;
	}

	rotation_.y += rightturnspeed_;

	if (rotation_.y > 360.0F)
	{
		rotation_.y -= 360.0F;
	}
}

void Position::lookUpWard(bool KeyDown)
{
	//キーが押されている間加速
	if (KeyDown)
	{
		lookupspeed_ = kLookspeed;
	}
	else
	{
		lookupspeed_ = 0.0F;
	}

	rotation_.x -= lookupspeed_;

	if (rotation_.x > 90.0F)
	{
		rotation_.x = 90.0F;
	}
}

void Position::lookDownWard(bool KeyDown)
{
	//キーが押されている間加速
	if (KeyDown)
	{
		lookdownspeed_ = kLookspeed;
	}
	else
	{
		lookdownspeed_ = 0.0F;
	}

	rotation_.x += lookdownspeed_;

	if (rotation_.x < -90.0F)
	{
		rotation_.x = -90.0F;
	}
}
