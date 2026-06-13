#include "pch.h"

#include "texture.h"
#include "system.h"

Texture::Texture() : 
    mTexture{nullptr},
    mWidth{0},
    mHeight{0}
{}

Texture::~Texture()
{
    Destroy();
}

void Texture::Destroy()
{
    //텍스처 정리
    SDL_DestroyTexture(mTexture);
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}

bool Texture::LoadFromRenderedText(std::string textureText, int size, SDL_Color textColor)
{
    Destroy();

    bool success = true;

    //텍스트 서페이스 로드
    SDL_Surface* textSurface = TTF_RenderText_Blended(System::sFont, textureText.c_str(), 0, textColor);
    SDL_Surface* scaledSurface = nullptr;
    if (textSurface == nullptr) {
        SDL_Log(SDL_GetError());
        success = false;
    }

    //사이즈가 0이 아니면 서페이스 스케일링
    if (size != 0) {
        if ((textureText[0] & 0b11110000) == 0b11100000) {
            scaledSurface = SDL_ScaleSurface(textSurface, size, size, SDL_SCALEMODE_LINEAR);
        }
        else {
            scaledSurface = SDL_ScaleSurface(textSurface, static_cast<int>(size/2), size, SDL_SCALEMODE_LINEAR);
        }
    }

    //서페이스에서 텍스처를 생성
    mTexture = SDL_CreateTextureFromSurface(System::sRenderer, textSurface);
    if (mTexture == nullptr) {
        SDL_Log(SDL_GetError());
        success = false;
    }

    if (success) {
        mWidth = textSurface->w;
        mHeight = textSurface->h;
    }

    //임시 서페이스 파괴
    SDL_DestroySurface(textSurface);
    SDL_DestroySurface(scaledSurface);

    return success;
}

void Texture::Render(float x, float y, SDL_FRect *clip, float width, float height, double degrees, SDL_FPoint *center, SDL_FlipMode flipmode)
{
    //텍스처 위치 설정
    SDL_FRect dstRect {x, y, static_cast<float>(mWidth), static_cast<float>(mHeight)};

    if (clip != nullptr) {
        dstRect.w = clip->w;
        dstRect.h = clip->h;
    }

    if (width > 0) {
        dstRect.w = width;
    }
    if (height > 0) {
        dstRect.h = height;
    }

    //텍스처 렌더링
    SDL_RenderTextureRotated(System::sRenderer, mTexture, clip, &dstRect, degrees, center, flipmode);
}

int Texture::GetWidth()
{
    return mWidth;
}

int Texture::GetHeight()
{
    return mHeight;
}

bool Texture::isLoaded()
{
    return mTexture != nullptr;
}
