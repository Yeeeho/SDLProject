#include "pch.h"

#include "system.h"
#include "camera.h"

Camera::Camera(unsigned int velocity, int xlimit, int ylimit)
{
    mSight = {0.f, 0.f, (float) System::sWindowWidth, (float) System::sWindowHeight};

    mXLimit = xlimit; mYLimit = ylimit;
    mVelocity = velocity;
}

void Camera::Move()
{
    mSight.x += mXSpeed;
    mSight.y += mYSpeed;

    //clamp
    if (mSight.x < 0) mSight.x = 0;
    if (mSight.x + mSight.w > mXLimit) mSight.x = mXLimit - mSight.w;
    
    if (mSight.y < 0) mSight.y = 0;
    if (mSight.y + mSight.h > mYLimit) mSight.y = mYLimit - mSight.h;
}
