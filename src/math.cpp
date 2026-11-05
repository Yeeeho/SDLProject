#include "pch.h"

#include "math.h"
#include "system/system.h"

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
        Log::LogVerbose("Move south east", System::sLogOptionMathOn);
        direction = Direction::SE;
    }
    if (vectorX > 0 && vectorY < 0) {
        Log::LogVerbose("Move north east", System::sLogOptionMathOn);
        direction = Direction::NE;
    }
    if (vectorX < 0 && vectorY > 0) {
        Log::LogVerbose("Move south west", System::sLogOptionMathOn);
        direction = Direction::SW;
    }
    if (vectorX < 0 && vectorY < 0) {
        Log::LogVerbose("Move north west", System::sLogOptionMathOn);
        direction = Direction::NW;
    }
    if (vectorX == 0) {
        //타겟과 액터가 y축에 나란히 있음
        if (vectorY < 0) {
        Log::LogVerbose("Move north", System::sLogOptionMathOn);
            direction = Direction::North;
        }
        if (vectorY > 0) {
        Log::LogVerbose("Move south", System::sLogOptionMathOn);
            direction = Direction::South;
        }
    }
    if (vectorY == 0) {
        //타겟과 액터가 x축에 나란히 있음
        if (vectorX < 0) {
        Log::LogVerbose("Move west", System::sLogOptionMathOn);
            direction = Direction::West;
        }
        if (vectorX > 0) {
        Log::LogVerbose("Move east", System::sLogOptionMathOn);
            direction = Direction::East;
        }
    }

    return direction;
}

Direction8::Direction8(Direction initDir)
{
    mDir = initDir;
}

Direction Direction8::GoCW()
{
    for (int i = 0; i< (int) sDirections.size(); i++) {
        if (sDirections[i] == mDir) {

            if (i == (int) sDirections.size() - 1) {
                mDir = sDirections[0];
                return sDirections[0];
            }
            mDir = sDirections[i+1];
            return sDirections[i+1];
        }
    }

    SDL_Log("[WARNING] GoCW: Unexpected direction");
    return Direction::None;
}

Direction Direction8::GoCCW()
{
    for (int i = 0; i< (int) sDirections.size(); i++) {
        if (sDirections[i] == mDir) {

            if (i == 0) {
                mDir = sDirections[(int) sDirections.size()-1];
                return sDirections[(int) sDirections.size()-1];
            }
            mDir = sDirections[i-1];
            return sDirections[i-1];
        }
    }

    SDL_Log("[WARNING] GoCCW: Unexpected direction");
    return Direction::None;
}
