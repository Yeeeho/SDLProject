#pragma once

#include <string>
#include <SDL3/SDL.h>

class Texture {
    public:

    static constexpr float kOriginalSize = -1.f;

    Texture();
    ~Texture();

    void Destroy();

    //텍스트 로드
    bool LoadFromRenderedText(std::string textureText, SDL_Color textColor);

    //렌더링
    void Render(float x, float y, SDL_FRect *clip = nullptr,
        float width = kOriginalSize, float height = kOriginalSize,
        double degrees = 0.0, SDL_FPoint* center = nullptr, SDL_FlipMode flipmode = SDL_FLIP_NONE);


    //텍스처 속성 가져오기
    int GetWidth();
    int GetHeight();
    bool isLoaded();

    private:
    SDL_Texture* mTexture;

    //텍스처 크기
    int mWidth;
    int mHeight;
};