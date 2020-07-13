#include "stdafx.h"
#include "Light.h"
#include "Input.h"

Light::Light()
{
}


Light::~Light()
{
}

void Light::update()
{
#ifdef _DEBUG
    if (Input::getInstance()->isKeyState(DIK_NUMPAD4))
        position_.x -= 0.1F;
    if (Input::getInstance()->isKeyState(DIK_NUMPAD6))
        position_.x += 0.1F;
    if (Input::getInstance()->isKeyState(DIK_NUMPAD8))
        position_.z += 0.1F;
    if (Input::getInstance()->isKeyState(DIK_NUMPAD2))
        position_.z -= 0.1F;
    if (Input::getInstance()->isKeyState(DIK_NUMPADENTER))
        position_.y -= 0.1F;
    if (Input::getInstance()->isKeyState(DIK_NUMPADPLUS))
        position_.y += 0.1F;
#endif
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
    projection_ = XMMatrixPerspectiveFovLH(fieldofview, screenaspect, ScreenNear, 10000.0F);
}

void Light::generateOrthoMatrix(const float Width, const float DepthPlane, const float NearPlane)
{
    ortho_ = XMMatrixOrthographicLH(Width, Width, NearPlane, DepthPlane);
}
