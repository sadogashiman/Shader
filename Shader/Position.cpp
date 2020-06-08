#include "stdafx.h"
#include "Position.h"

Position::Position()
{
    ZeroMemory(this, sizeof(Position));
}

void Position::moveForWard(bool KeyDown)
{
    float radian;

    //キーが押されている間加速
    if (KeyDown)
    {
        forwardspeed_ += frametime_ * 0.001F;
        if (forwardspeed_ > (frametime_ * 0.03F))
        {
            forwardspeed_ = frametime_ * 0.03F;
        }
    }
    else
    {
        forwardspeed_ -= frametime_ * 0.0007F;

        if (forwardspeed_ < 0.0F)
        {
            forwardspeed_ = 0.0F;
        }

    }

    //ラジアンに変換して代入
    radian = XMConvertToRadians(rotation_.y);

    position_.x += sin(radian) * forwardspeed_;
    position_.z += cos(radian) * forwardspeed_;
}

void Position::moveBackWard(bool KeyDown)
{
    float radian;

    //キーが押されている間加速
    if (KeyDown)
    {
        backwardspeed_ += frametime_ * 0.001F;
        if (backwardspeed_ > (frametime_ * 0.03F))
        {
            backwardspeed_ = frametime_ * 0.03F;
        }
    }
    else
    {
        backwardspeed_ -= frametime_ * 0.0007F;

        if (backwardspeed_ < 0.0F)
        {
            backwardspeed_ = 0.0F;
        }
    }

    //ラジアンに変換して代入
    radian = XMConvertToRadians(rotation_.y);

    position_.x -= sin(radian) * backwardspeed_;
    position_.z -= cos(radian) * backwardspeed_;
}

void Position::moveUpWard(bool KeyDown)
{
    //キーが押されている間加速
    if (KeyDown)
    {
        upwardspeed_ += frametime_ * 0.003F;
        if (upwardspeed_ > (frametime_ * 0.03F))
        {
            upwardspeed_ = frametime_ * 0.03F;
        }
    }
    else
    {
        upwardspeed_ -= frametime_ * 0.002F;
        if (upwardspeed_ < 0.0F)
        {
            upwardspeed_ = 0.0F;
        }
    }

    position_.y += upwardspeed_;
}

void Position::moveDownWard(bool KeyDown)
{
    //キーが押されている間加速
    if (KeyDown)
    {
        downwardspeed_ += frametime_ * 0.003F;
        if (downwardspeed_ > (frametime_ * 0.03F))
        {
            downwardspeed_ = frametime_ * 0.03F;
        }
    }
    else
    {
        downwardspeed_ -= frametime_ * 0.002F;

        if (downwardspeed_ < 0.0F)
        {
            downwardspeed_ = 0.0F;
        }
    }

    position_.y -= downwardspeed_;
}

void Position::turnLeft(bool KeyDown)
{
    //キーが押されている間加速
    if (KeyDown)
    {
        leftturnspeed_ += frametime_ * 0.01F;

        if (forwardspeed_ > (frametime_ * 0.15F))
        {
            forwardspeed_ = frametime_ * 0.15F;
        }
    }
    else
    {
        leftturnspeed_ -= frametime_ * 0.005F;

        if (leftturnspeed_ < 0.0F)
        {
            leftturnspeed_ = 0.0F;
        }
    }

    rotation_.y -= leftturnspeed_;

    if (rotation_.y < 0.0F)
    {
        rotation_.y += 360.0F;
    }
}

void Position::turnRight(bool KeyDown)
{
    //キーが押されている間加速
    if (KeyDown)
    {
        rightturnspeed_ += frametime_ * 0.01F;

        if (rightturnspeed_ > (frametime_ * 0.15F))
        {
            rightturnspeed_ = frametime_ * 0.15F;
        }
    }
    else
    {
        rightturnspeed_ -= frametime_ * 0.005F;

        if (rightturnspeed_ < 0.0F)
        {
            rightturnspeed_ = 0.0F;
        }
    }

    rotation_.y += rightturnspeed_;

    if (rotation_.y > 360.0F)
    {
        rotation_.y -= 360.0F;
    }
}

void Position::lookUpWard(bool KeyDown)
{
    //キーが押されている間加速
    if (KeyDown)
    {
        lookupspeed_ += frametime_ * 0.01F;
        if (forwardspeed_ > (frametime_ * 0.15F))
        {
            forwardspeed_ = frametime_ * 0.15F;
        }
    }
    else
    {
        lookupspeed_ -= frametime_ * 0.005F;

        if (lookupspeed_ < 0.0F)
        {
            lookupspeed_ = 0.0F;
        }
    }

    rotation_.x -= lookupspeed_;

    if (rotation_.x > 90.0F)
    {
        rotation_.x = 90.0F;
    }
}

void Position::lookDownWard(bool KeyDown)
{
    //キーが押されている間加速
    if (KeyDown)
    {
        lookdownspeed_ += frametime_ * 0.01F;
        if (forwardspeed_ > (frametime_ * 0.15F))
        {
            forwardspeed_ = frametime_ * 0.15F;
        }
    }
    else
    {
        lookdownspeed_ -= frametime_ * 0.005F;

        if (lookdownspeed_ < 0.0F)
        {
            lookdownspeed_ = 0.0F;
        }
   }

    rotation_.x += lookdownspeed_;

    if (rotation_.x <- 90.0F)
    {
        rotation_.x = -90.0F;
    }
}
