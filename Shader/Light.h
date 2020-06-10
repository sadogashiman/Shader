#pragma once
class Light
{
private:
	Vector4 diffusecolor_;
	Vector4 ambientcolor_;
	Vector4 specularcolor_;
	Vector3 direction_;
	Vector3 lookat_;
	Vector3 position_;
	float specularpower_;
	Matrix view_;
	Matrix projection_;
public:
	Light();
	~Light();

	//set
	inline void setDiffuseColor(const float Red, const float Green, const float Blue, const float Alpha) { diffusecolor_ = Vector4(Red, Blue, Green, Alpha); }
	inline void setDirection(const float X, const float Y, const float Z) { direction_ = Vector3(X, Y, Z); }
	inline void setAmbientColor(const float Red, const float Green, const float Blue, const float Alpha) { ambientcolor_ = Vector4(Red, Blue, Green, Alpha); }
	inline void setSpecularColor(const float Red, const float Green, const float Blue, const float Alpha) { specularcolor_ = Vector4(Red, Blue, Green, Alpha); }
	inline void setSpecularPower(const float SpecularPower) { specularpower_ = SpecularPower; }
	inline void setLookAt(const float X, const float Y, const float Z) { lookat_ = Vector3(X, Y, Z); }
	inline void setPosition(const float X,const float Y,const float Z) { position_ = Vector3(X, Y, Z); }
	
	//get
	inline Vector4 getAmbientColor()const {return ambientcolor_;}
	inline Vector4 getDiffuseColor()const { return diffusecolor_; }
	inline Vector4 getSpecularColor()const { return specularcolor_; }
	inline Vector3 getDirection()const { return direction_; }
	inline Vector3 getPosition()const { return position_; }
	inline float getSpecularPower()const { return specularpower_; }
	inline Matrix getViewMatrix()const {return view_;}
	inline Matrix getProjection()const { return projection_; }

	//generate
	void generateView();
	void generateProjection(const float ScreenDepth, const float ScreenNear);



};

