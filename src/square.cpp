#include "pch.h"
#include <SDL3/SDL.h>

#include "system.h"
#include "square.h"

float Square::GetX()
{
    return mX;
}

float Square::SetX(float x)
{
    mX = x;
    return mX;
}

float Square::GetY()
{
    return mY;
}

float Square::SetY(float y)
{
    mY = y;
    return mY;
}

int Square::GetW()
{
    return mWidth;
}

int Square::SetW(int w)
{
    mWidth = w;
    return mWidth;
}

int Square::GetH()
{
    return mHeight;
}

int Square::SetH(int h)
{
    mHeight = h;
    return mHeight;
}

void Square::SetColor(SDL_Color fillcolor, SDL_Color linecolor)
{
    mFillColor = fillcolor;
    mLineColor = linecolor;
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

void Square::RenderColored()
{
    SDL_FRect drawingRect{static_cast<float>(mX), static_cast<float>(mY), static_cast<float>(mWidth), static_cast<float>(mHeight)};

    SDL_SetRenderDrawColor(System::sRenderer, mFillColor.r, mFillColor.g, mFillColor.b, mFillColor.a);
    SDL_RenderFillRect(System::sRenderer, &drawingRect);
    //테두리 
    SDL_SetRenderDrawColor(System::sRenderer, mLineColor.r, mLineColor.g, mLineColor.b, mLineColor.a);
    SDL_RenderRect(System::sRenderer, &drawingRect);
}

void Square::Render(SDL_Color lineColor, SDL_Color fillColor)
{
    SDL_FRect drawingRect{static_cast<float>(mX), static_cast<float>(mY), static_cast<float>(mWidth), static_cast<float>(mHeight)};
    
    //채우기
    SDL_SetRenderDrawColor(System::sRenderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    SDL_RenderFillRect(System::sRenderer, &drawingRect);
    //테두리
    SDL_SetRenderDrawColor(System::sRenderer, lineColor.r, lineColor.g, lineColor.b, lineColor.a);
    SDL_RenderRect(System::sRenderer, &drawingRect);
}

void Square::Render(Uint8 lineR, Uint8 lineG, Uint8 lineB, Uint8 lineA, Uint8 fillR, Uint8 fillG, Uint8 fillB, Uint8 fillA)
{
    SDL_FRect drawingRect{static_cast<float>(mX), static_cast<float>(mY), static_cast<float>(mWidth), static_cast<float>(mHeight)};

    //채우기
    SDL_SetRenderDrawColor(System::sRenderer, fillR, fillG, fillB, fillA);
    SDL_RenderFillRect(System::sRenderer, &drawingRect);
    //테두리
    SDL_SetRenderDrawColor(System::sRenderer, lineR, lineG, lineB, lineA);
    SDL_RenderRect(System::sRenderer, &drawingRect);
}
