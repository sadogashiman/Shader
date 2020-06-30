#include "stdafx.h"
#include "Camera.h"
#include "Timer.h"
#include "Input.h"

Camera::Camera()
{
	ZeroMemory(this, sizeof(Camera));
}

Camera::~Camera()
{
}

bool Camera::init()
{
	//���W�Ǘ��N���X�𐶐�
	poscontroller_.reset(new Position);
	if (!poscontroller_.get())
	{
		return false;
	}

	return true;
}

void Camera::update()
{
	//�J��������
	poscontroller_.get()->setFrameTime(16.0F);

	poscontroller_.get()->moveRightWard(Input::getInstance()->isKeyState(DIK_D));
	poscontroller_.get()->moveLeftWard(Input::getInstance()->isKeyState(DIK_A));
	poscontroller_.get()->moveForWard(Input::getInstance()->isKeyState(DIK_W));
	poscontroller_.get()->moveBackWard(Input::getInstance()->isKeyState(DIK_S));
	poscontroller_.get()->moveUpWard(Input::getInstance()->isKeyState(DIK_UP));
	poscontroller_.get()->moveDownWard(Input::getInstance()->isKeyState(DIK_DOWN));
	poscontroller_.get()->turnLeft(Input::getInstance()->isKeyState(DIK_LEFT));
	poscontroller_.get()->turnRight(Input::getInstance()->isKeyState(DIK_RIGHT));
	poscontroller_.get()->lookUpWard(Input::getInstance()->isKeyState(DIK_PGUP));
	poscontroller_.get()->lookDownWard(Input::getInstance()->isKeyState(DIK_PGDN));

	setPosition(poscontroller_.get()->getPosition());
	setRotation(poscontroller_.get()->getRotation());
}

void Camera::render()
{
	Vector3 up, position,lookat;
	float yaw, pitch, roll;
	Matrix rotationmatrix;

	//������̃x�N�g���ɐݒ�
	up.x = 0.0F;
	up.y = 1.0F;
	up.z = 0.0F;

	//���[���h���̃J�������W���v�Z
	position = position_;

	//�J�����̏����ʒu��ݒ�
	lookat.x = 0.0F;
	lookat.y = 0.0F;
	lookat.z = 1.0F;

	//���W�A���ŃJ�����̉�]�n��ݒ�
	pitch = XMConvertToRadians(rotation_.x);
	yaw = XMConvertToRadians(rotation_.y);
	roll = XMConvertToRadians(rotation_.z);

	//�ϊ������l�ŉ�]�s����쐬
	rotationmatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//�r���[�����_�Ő�������]����悤�ɉ�]�s��ɂ����lookat��up�x�N�g����ϊ�
	lookat = XMVector3TransformCoord(lookat, rotationmatrix);
	up = XMVector3TransformCoord(up, rotationmatrix);

	//�J�X�����J�����̈ʒu���r���[�A�[�̈ʒu�ɕϊ�
	lookat += position;

	//�X�V���ꂽ�x�N�g������r���[�s����쐬
	view_ = XMMatrixLookAtLH(position, lookat, up);
}

void Camera::renderBaseViewMatrix()
{
	Vector3 up, position, lookat;
	float yaw, pitch, roll;
	Matrix rotationmatrix;

	//������̃x�N�g�����v�Z
	up.x = 0.0F;
	up.y = 1.0F;
	up.z = 0.0F;

	//���[���h���̃J�������W���v�Z
	position = position_;

	//�J�����̏����ʒu��ݒ�
	lookat.x = 0.0F;
	lookat.y = 0.0F;
	lookat.z = 1.0F;

	//���W�A���ŃJ�����̉�]�n��ݒ�
	pitch = XMConvertToRadians(rotation_.x);
	yaw = XMConvertToRadians(rotation_.y);
	roll = XMConvertToRadians(rotation_.z);

	//�r���[�����_�Ő�������]����悤�ɉ�]�s��ɂ����lookat��up�x�N�g����ϊ�
	lookat = XMVector3TransformCoord(lookat, rotationmatrix);
	up = XMVector3TransformCoord(up, rotationmatrix);

	//��]�����J�����̈ʒu���r���[�A�[�̈ʒu�ɕϊ�
	lookat = position + lookat;

	//�X�V���ꂽ�x�N�g������r���[�s����쐬
	baseview_ = XMMatrixLookAtLH(position, lookat, up);
}