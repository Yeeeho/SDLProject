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

int Square::GetW()
{
    return mWidth;
}

int Square::GetH()
{
    return mHeight;
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
    
    //채우기
    SDL_SetRenderDrawColor(System::sRenderer, 0x08, 0x08, 0x08, 0x10);
    SDL_RenderFillRect(System::sRenderer, &drawingRect);
    //테두리 
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0xB0, 0x00, 0xFF);
    SDL_RenderRect(System::sRenderer, &drawingRect);
}
