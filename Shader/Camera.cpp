#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
{
	position_ = Vector3::Zero;
	rotation_ = Vector3::Zero;
}

Camera::~Camera()
{
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

void Camera::renderReflection(const float Height)
{
	Vector3 up, position, lookat;
	float radian;

	//������̃x�N�g�����v�Z
	up.x = 0.0F;
	up.y = 1.0F;
	up.z = 0.0F;

	//���ʔ��˂̏ꍇ�J�����̈ʒu�𔽓]
	position.x = position_.x;
	position.y = -position_.y + (Height * 2.0F);
	position.z = position_.z;

	//���W�A���P�ʂŉ�]���v�Z
	radian = XMConvertToRadians(rotation_.y);

	lookat.x = sinf(radian) + position_.x;
	lookat.y = position_.y;
	lookat.z = cosf(radian) + position_.z;

	//3�̃x�N�g������r���[�s����쐬
	reflectionview_ = XMMatrixLookAtLH(position_, lookat, up);
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

	//�J�X�����J�����̈ʒu���r���[�A�[�̈ʒu�ɕϊ�
	lookat += position + lookat;

	//�X�V���ꂽ�x�N�g������r���[�s����쐬
	baseview_ = XMMatrixLookAtLH(position, lookat, up);
}

void Camera::setPosition(const Vector3& Position)
{
	position_ = Position;
}

void Camera::setRotation(const Vector3& Rotation)
{
	rotation_ = Rotation;
}
