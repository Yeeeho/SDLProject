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

Direction Math::GetDirection8(int vectorX, int vectorY)
{
    Direction direction = Direction::None;

    if (vectorX > 0 && vectorY > 0) {
        SDL_Log("--> Move south east");
        direction = Direction::SE;
    }
    if (vectorX > 0 && vectorY < 0) {
        SDL_Log("--> Move north east");
        direction = Direction::NE;
    }
    if (vectorX < 0 && vectorY > 0) {
        SDL_Log("--> Move south west");
        direction = Direction::SW;
    }
    if (vectorX < 0 && vectorY < 0) {
        SDL_Log("--> Move north west");
        direction = Direction::NW;
    }
    if (vectorX == 0) {
        //타겟과 액터가 y축에 나란히 있음
        if (vectorY < 0) {
            SDL_Log("--> Move up");
                direction = Direction::North;
        }
        if (vectorY > 0) {
            SDL_Log("--> Move down");
                direction = Direction::South;
        }
    }
    if (vectorY == 0) {
        //타겟과 액터가 x축에 나란히 있음
        if (vectorX < 0) {
            SDL_Log("--> Move left");
                direction = Direction::West;
        }
        if (vectorX > 0) {
            SDL_Log("--> Move right");
                direction = Direction::East;
        }
    }

    return direction;
}
