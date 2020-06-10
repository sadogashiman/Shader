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

    //������̃x�N�g�����v�Z
    up.x = 0.0F;
    up.y = 1.0F;
    up.z = 0.0F;

    //�r���[�s��쐬
    view_ = XMMatrixLookAtLH(position_, lookat_, up);
}

void Light::generateProjection(const float ScreenDepth, const float ScreenNear)
{
    float fieldofview, screenaspect;

    //�����`�̍����̎���Ɖ�ʂ̃A�X�y�N�g��ݒ�
    fieldofview = static_cast<float>(XM_PI) / 2.0F;
    screenaspect = 1.0F;

    //���C�g�̓��e�s����쐬
    projection_ = XMMatrixPerspectiveFovLH(fieldofview, screenaspect, ScreenNear, ScreenDepth);

}
