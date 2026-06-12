#include "pch.h"
#include <SDL3/SDL.h>

#include "system.h"
#include "square.h"

int Square::GetX()
{
    return mX;
}

int Square::GetY()
{
    return mY;
}

Square::Square(int x, int y, int width, int height)
{
    mX = x;
    mY = y;
    mWidth = width;
    mHeight = height;
}

void Square::Render()
{
    SDL_FRect drawingRect{static_cast<float>(mX), static_cast<float>(mY), static_cast<float>(mWidth), static_cast<float>(mHeight)};
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0xFF, 0x00, 0xFF);
    SDL_RenderRect(System::sRenderer, &drawingRect);
}
