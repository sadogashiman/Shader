#include "stdafx.h"
#include "Light.h"


Light::Light()
{
    ZeroMemory(this, sizeof(Light));
}


Light::~Light()
{
}

void Light::setDiffuseColor(const float Red, const float Green, const float Blue, const float Alpha)
{
    diffusecolor_ = Vector4(Red, Green, Blue, Alpha);
}

void Light::setDirection(const float X, const float Y, const float Z)
{
    direction_ = Vector3(X, Y, Z);
}

void Light::setAmbientColor(const float Red, const float Green, const float Blue, const float Alpha)
{
    ambientcolor_ = Vector4(Red, Green, Blue, Alpha);
}

void Light::setSpecularColor(const float Red, const float Green, const float Blue, const float Alpha)
{
    specularcolor_ = Vector4(Red, Green, Blue, Alpha);
}

void Light::setSpecularPower(const float SpecularPower)
{
    specularpower_ = SpecularPower;
}

void Light::setLookAt(const float x, const float y, const float z)
{
    lookat_.x = x;
    lookat_.y = y;
    lookat_.z = z;
}

void Light::setPosition(float X, float Y, float Z)
{
    position_ = Vector3(X, Y, Z);
}

Vector4 Light::getAmbientColor() const
{
    return ambientcolor_;
}

Vector4 Light::getDiffuseColor() const
{
    return diffusecolor_;
}

Vector3 Light::getDirection() const
{
    return direction_;
}

Vector3 Light::getPosition() const
{
    return position_;
}

Vector4 Light::getSpecularColor() const
{
    return specularcolor_;
}

float Light::getSpecularPower() const
{
    return specularpower_;
}

void Light::generateView()
{
    Vector3 up;

    //上向きのベクトルを計算
    up.x = 0.0F;
    up.y = 1.0F;
    up.z = 0.0F;

    //ビュー行列作成
    view_ = XMMatrixLookAtLH(position_, lookat_, up);
}

void Light::generateProjection(const float ScreenDepth, const float ScreenNear)
{
    float fieldofview, screenaspect;

    //正方形の高原の視野と画面のアスペクトを設定
    fieldofview = static_cast<float>(XM_PI) / 2.0F;
    screenaspect = 1.0F;

    //ライトの投影行列を作成
    projection_ = XMMatrixPerspectiveFovLH(fieldofview, screenaspect, ScreenNear, ScreenDepth);

}
