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
    void SetAmbientColor(const float Red, const float Green, const float Blue, const float Alpha);
    void SetSpecularColor(const float Red, const float Green, const float Blue, const float Alpha);
    void SetSpecularPower(const float SpecularPower);
    void setLookAt(const float x, const float y, const float z);
    void setPosition(float X, float Y, float Z);
    //get
    Vector4 GetAmbientColor()const;
    Vector4 GetDiffuseColor()const;
    Vector4 GetSpecularColor()const;
    Vector3 GetDirection()const;
    Vector3 GetPosition()const;
    float GetSpecularPower()const;
    void getViewMatrix(Matrix& View);
    void getProjectionMatrix(Matrix& Projection);

    //generate
    void generateView();
    void generateProjection(const float ScreenDepth, const float ScreenNear);



};

