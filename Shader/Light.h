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
	Matrix ortho_;
	float range_;
	float cone_;
	Vector3 attenuator_;
public:
	Light();
	~Light();

	void update();

	//set
	inline void setDiffuseColor(const float Red, const float Green, const float Blue, const float Alpha) { diffusecolor_ = Vector4(Red, Blue, Green, Alpha); }
	inline void setDiffuseColor(const Vector4 diffuseColor) { diffusecolor_ = diffuseColor; }
	inline void setDirection(const float X, const float Y, const float Z) { direction_ = Vector3(X, Y, Z); }
	inline void setDirection(const Vector3 Direction) { direction_ = Direction; }
	inline void setAmbientColor(const float Red, const float Green, const float Blue, const float Alpha) { ambientcolor_ = Vector4(Red, Blue, Green, Alpha); }
	inline void setAmbientColor(const Vector4 ambientColor) { ambientcolor_ = ambientColor; }
	inline void setSpecularColor(const float Red, const float Green, const float Blue, const float Alpha) { specularcolor_ = Vector4(Red, Blue, Green, Alpha); }
	inline void setSpecularColor(const Vector4 SpecularColor) { specularcolor_ = SpecularColor; }
	inline void setSpecularPower(const float SpecularPower) { specularpower_ = SpecularPower; }
	inline void setLookAt(const float X, const float Y, const float Z) { lookat_ = Vector3(X, Y, Z); }
	inline void setLookAt(const Vector3 Lookat) { lookat_ = Lookat; }
	inline void setPosition(const float X,const float Y,const float Z) { position_ = Vector3(X, Y, Z); }
	inline void setPosition(const Vector3 Position) { position_ = Position; }
	inline void setLightRange(const float Range) { range_ = Range; }
	inline void setConeSize(const float ConeSize) { cone_ = ConeSize; }
	inline void setAttenuator(const Vector3 Att) { attenuator_ = Att; }
	inline void setAttenuator(const float X, const float Y, const float Z) { attenuator_ = Vector3(X, Y, Z); }

	//get
	inline Vector4 getAmbientColor()const {return ambientcolor_;}
	inline Vector4 getDiffuseColor()const { return diffusecolor_; }
	inline Vector4 getSpecularColor()const { return specularcolor_; }
	inline Vector3 getDirection()const { return direction_; }
	inline Vector3 getPosition()const { return position_; }
	inline float getSpecularPower()const { return specularpower_; }
	inline Matrix getViewMatrix()const {return view_;}
	inline Matrix getProjectionMatrix()const { return projection_; }
	inline Matrix getOrthoMatrix()const { return ortho_; }
	inline float getLightRange()const { return range_; }
	inline float getLightCone()const { return cone_; }
	inline Vector3 getAttenuator()const { return attenuator_; }


	//generate
	void generateView();
	void generateProjection(const float ScreenDepth, const float ScreenNear);
	void generateOrthoMatrix(const float Width, const float DepthPlane, const float NearPlane);
};

