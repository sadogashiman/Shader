#include "stdafx.h"
#include "Position.h"
const float kMovespeed = 1.0F;
const float kTurnspeed = 1.0F;
const float kLookspeed = 1.0F;

Position::Position()
{
	ZeroMemory(this, sizeof(Position));
	position_.y = 2.0F;
	position_.x = 50.0F;
}

void Position::moveForWard(bool KeyDown)
{
	float radian;
	float forwardspeed;

	//キーが押されている間加速
	if (KeyDown)
	{
		forwardspeed = kMovespeed;
	}
	else
	{
		forwardspeed = 0.0F;
	}

	//ラジアンに変換して代入
	radian = XMConvertToRadians(rotation_.y);

	position_.x += sin(radian) * forwardspeed;
	position_.z += cos(radian) * forwardspeed;
}

void Position::moveBackWard(bool KeyDown)
{
	float radian;
	float backwardspeed;

	//キーが押されている間加速
	if (KeyDown)
	{
		backwardspeed = kMovespeed;
	}
	else
	{
		backwardspeed = 0.0F;
	}

	//ラジアンに変換して代入
	radian = XMConvertToRadians(rotation_.y);

	position_.x -= sin(radian) * backwardspeed;
	position_.z -= cos(radian) * backwardspeed;
}

void Position::moveUpWard(bool KeyDown)
{
	float upwardspeed;

	//キーが押されている間加速
	if (KeyDown)
	{
		upwardspeed = kMovespeed;
	}
	else
	{
		upwardspeed = 0.0F;
	}

	position_.y += upwardspeed;
}

void Position::moveDownWard(bool KeyDown)
{
	float downwardspeed;

	//キーが押されている間加速
	if (KeyDown)
	{
		downwardspeed = kMovespeed;
	}
	else
	{
		downwardspeed = 0.0F;

	}

	position_.y -= downwardspeed;
}

void Position::moveLeftWard(bool KeyDown)
{
	float leftwardspeed;
	float radian;
	float offset = -270.0F;

	if (KeyDown)
	{
		leftwardspeed = kMovespeed / 2;
	}
	else
	{
		leftwardspeed = 0;
	}

	radian = XMConvertToRadians(rotation_.y + offset);

	position_.x -= sin(radian) * leftwardspeed;
	position_.z -= cos(radian) * leftwardspeed;
}

void Position::moveRightWard(bool KeyDown)
{
	float radian;
	float rightwardspeed;
	float offset = 90.0F;

	//キーが押されている間加速
	if (KeyDown)
	{
		rightwardspeed = kMovespeed / 2;
	}
	else
	{
		rightwardspeed = 0.0F;
	}

	//ラジアンに変換して代入
	radian = XMConvertToRadians(rotation_.y + offset);

	position_.x += sin(radian) * rightwardspeed;
	position_.z += cos(radian) * rightwardspeed;
}

void Position::turnLeft(bool KeyDown)
{
	float leftturnspeed;

	//キーが押されている間加速
	if (KeyDown)
	{
		leftturnspeed = kTurnspeed;
	}
	else
	{
		leftturnspeed = 0.0F;

	}

	rotation_.y -= leftturnspeed;

	if (rotation_.y < 0.0F)
	{
		rotation_.y += 360.0F;
	}
}

void Position::turnRight(bool KeyDown)
{
	float rightturnspeed;

	//キーが押されている間加速
	if (KeyDown)
	{
		rightturnspeed = kTurnspeed;
	}
	else
	{
		rightturnspeed = 0.0F;
	}

	rotation_.y += rightturnspeed;

	if (rotation_.y > 360.0F)
	{
		rotation_.y -= 360.0F;
	}
}

void Position::lookUpWard(bool KeyDown)
{
	float lookupspeed;

	//キーが押されている間加速
	if (KeyDown)
	{
		lookupspeed = kLookspeed;
	}
	else
	{
		lookupspeed = 0.0F;
	}

	rotation_.x -= lookupspeed;

	if (rotation_.x > 90.0F)
	{
		rotation_.x = 90.0F;
	}
}

void Position::lookDownWard(bool KeyDown)
{
	float lookdownspeed;

	//キーが押されている間加速
	if (KeyDown)
	{
		lookdownspeed = kLookspeed;
	}
	else
	{
		lookdownspeed = 0.0F;
	}

	rotation_.x += lookdownspeed;

	if (rotation_.x < -90.0F)
	{
		rotation_.x = -90.0F;
	}
}
