#include "pch.h"

#include "math.h"

//제곧내
bool Math::IsPointInSquare(float mouseX, float mouseY, float mX, float mY, float mW, float mH)
{
    bool pointIn = false;

    if (mouseX < mX) return pointIn;
    if (mouseX > mX + mW) return pointIn;
    if (mouseY < mY) return pointIn;
    if (mouseY > mY + mH) return pointIn;

    //마우스가 안에 있음
    pointIn = true;
    return pointIn;
}
