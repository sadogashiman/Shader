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
	//座標管理クラスを生成
	poscontroller_.reset(new Position);
	if (!poscontroller_.get())
	{
		return false;
	}

	return true;
}

void Camera::update()
{
	//カメラ制御
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

	//上向きのベクトルに設定
	up.x = 0.0F;
	up.y = 1.0F;
	up.z = 0.0F;

	//ワールド内のカメラ座標を計算
	position = position_;

	//カメラの初期位置を設定
	lookat.x = 0.0F;
	lookat.y = 0.0F;
	lookat.z = 1.0F;

	//ラジアンでカメラの回転地を設定
	pitch = XMConvertToRadians(rotation_.x);
	yaw = XMConvertToRadians(rotation_.y);
	roll = XMConvertToRadians(rotation_.z);

	//変換した値で回転行列を作成
	rotationmatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//ビューが原点で正しく回転するように回転行列によってlookatとupベクトルを変換
	lookat = XMVector3TransformCoord(lookat, rotationmatrix);
	up = XMVector3TransformCoord(up, rotationmatrix);

	//開店したカメラの位置をビューアーの位置に変換
	lookat += position;

	//更新されたベクトルからビュー行列を作成
	view_ = XMMatrixLookAtLH(position, lookat, up);
}

void Camera::renderBaseViewMatrix()
{
	Vector3 up, position, lookat;
	float yaw, pitch, roll;
	Matrix rotationmatrix;

	//上向きのベクトルを計算
	up.x = 0.0F;
	up.y = 1.0F;
	up.z = 0.0F;

	//ワールド内のカメラ座標を計算
	position = position_;

	//カメラの初期位置を設定
	lookat.x = 0.0F;
	lookat.y = 0.0F;
	lookat.z = 1.0F;

	//ラジアンでカメラの回転地を設定
	pitch = XMConvertToRadians(rotation_.x);
	yaw = XMConvertToRadians(rotation_.y);
	roll = XMConvertToRadians(rotation_.z);

	//ビューが原点で正しく回転するように回転行列によってlookatとupベクトルを変換
	lookat = XMVector3TransformCoord(lookat, rotationmatrix);
	up = XMVector3TransformCoord(up, rotationmatrix);

	//回転したカメラの位置をビューアーの位置に変換
	lookat = position + lookat;

	//更新されたベクトルからビュー行列を作成
	baseview_ = XMMatrixLookAtLH(position, lookat, up);
}