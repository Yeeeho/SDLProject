#include "pch.h"

#include "texture.h"
#include "system.h"


SDL_Texture* TextureManager::CreateTempTexture()
{   
    SDL_Texture* texture = SDL_CreateTexture(
        System::sRenderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        System::sWindowWidth, System::sWindowHeight
    );

    return texture;
}

SDL_Texture *TextureManager::CreateTempTexture(SDL_Renderer* rd, int w, int h)
{
    SDL_Texture* texture = SDL_CreateTexture(
        rd,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        w, h
    );

    return texture;
}

void TextureManager::Resize(SDL_Texture *target, SDL_Texture *source)
{
    float targetW = 0.f, targetH = 0.f;
    SDL_GetTextureSize(target, &targetW, &targetH);
    std::string message = std::to_string(targetW) + " " + std::to_string(targetH);
    SDL_Log(message.c_str());

    float w = 0.f, h = 0.f;
    SDL_GetTextureSize(source, &w, &h);
    message = std::to_string(w) + " " + std::to_string(h);
    SDL_Log(message.c_str());

    if (targetW != w || targetH != h) {
        SDL_DestroyTexture(source);
    
        source = CreateTempTexture(System::sRenderer, (int) targetW, (int) targetH);
    }
}

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
    if (mTexture != nullptr) {
        SDL_DestroyTexture(mTexture);
        mTexture = nullptr;
        mWidth = 0;
        mHeight = 0;
    }
}

bool Texture::LoadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font)
{
    Destroy();

    bool success = true;

    //텍스트 서페이스 로드
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, textureText.c_str(), 0, textColor);
    if (textSurface == nullptr) {
        SDL_Log(SDL_GetError());
        success = false;
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
    return success;
}

bool Texture::LoadFromFile(std::string path)
{
    Destroy();

    //서페이스 로드
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        SDL_Log("unable to load %s", path);
        SDL_Log(SDL_GetError());
    }
    else {  
        mTexture = SDL_CreateTextureFromSurface(System::sRenderer, loadedSurface);
        if (mTexture == nullptr) {
            SDL_Log(SDL_GetError());
        }
        else {
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }

        SDL_DestroySurface(loadedSurface);
    }


    return mTexture != nullptr;
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


