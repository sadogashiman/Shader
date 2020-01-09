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
    void setDiffuseColor(const float Red, const float Green, const float Blue, const float Alpha);
    void setDirection(const float X, const float Y, const float Z);
    void setAmbientColor(const float Red, const float Green, const float Blue, const float Alpha);
    void setSpecularColor(const float Red, const float Green, const float Blue, const float Alpha);
    void setSpecularPower(const float SpecularPower);
    void setLookAt(const float x, const float y, const float z);
    void setPosition(float X, float Y, float Z);
    //get
    Vector4 getAmbientColor()const;
    Vector4 getDiffuseColor()const;
    Vector4 getSpecularColor()const;
    Vector3 getDirection()const;
    Vector3 getPosition()const;
    float getSpecularPower()const;
	inline Matrix getViewMatrix()const {return view_;}
	inline Matrix getProjection()const { return projection_; }

    //generate
    void generateView();
    void generateProjection(const float ScreenDepth, const float ScreenNear);



};

