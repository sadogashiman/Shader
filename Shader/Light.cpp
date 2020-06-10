#include "stdafx.h"
#include "Light.h"


Light::Light()
{
    ZeroMemory(this, sizeof(Light));
}


Light::~Light()
{
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
