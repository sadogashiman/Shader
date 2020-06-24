#pragma once
#include "Position.h"
class Camera
{
private:
	Vector3 position_;
	Vector3 rotation_;
	Matrix view_;
	Matrix reflectionview_;
	Matrix baseview_;
	Position poscontroller_;
public:
	Camera();
	~Camera();

	//Move
	void moveCamera();

	//Render
	void render();
	void renderReflection(const float Height);
	void renderBaseViewMatrix();

	//Set
	inline void setPosition(const Vector3& Position) { position_ = Position; }
	inline void setRotation(const Vector3& Rotation) { rotation_ = Rotation; }

	//Get
	inline const Vector3 getPosition()const { return position_; }
	inline const Vector3 getRotation()const { return rotation_; }
	inline const Matrix getViewMatrix()const { return view_; }
	inline const Matrix getReflectionViewMatrix()const { return reflectionview_; }
	inline const Matrix getBaseViewMatrix()const { return baseview_; }

};

