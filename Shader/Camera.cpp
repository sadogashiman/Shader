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

void Camera::moveCamera()
{

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

void Camera::renderReflection(const float Height)
{
	Vector3 up, position, lookat;
	float radian;

	//上向きのベクトルを計算
	up.x = 0.0F;
	up.y = 1.0F;
	up.z = 0.0F;

	//平面反射の場合カメラの位置を反転
	position.x = position_.x;
	position.y = -position_.y + (Height * 2.0F);
	position.z = position_.z;

	//ラジアン単位で回転を計算
	radian = XMConvertToRadians(rotation_.y);

	lookat.x = sinf(radian) + position_.x;
	lookat.y = position_.y;
	lookat.z = cosf(radian) + position_.z;

	//3つのベクトルからビュー行列を作成
	reflectionview_ = XMMatrixLookAtLH(position_, lookat, up);
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

	//開店したカメラの位置をビューアーの位置に変換
	lookat += position + lookat;

	//更新されたベクトルからビュー行列を作成
	baseview_ = XMMatrixLookAtLH(position, lookat, up);
}