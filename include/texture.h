#pragma once

#include <string>
#include <vector>
#include <SDL3/SDL.h>

class TextureManager {
    public:
    SDL_Texture* CreateTempTexture();
    SDL_Texture* CreateTempTexture(SDL_Renderer* rd, int w, int h);

    void Resize(SDL_Texture* target, SDL_Texture* source);
};

class Texture {
    public:

    static constexpr float kOriginalSize = -1.f;

    Texture();
    Texture(std::string path);
    ~Texture();

    void Destroy();

    //텍스트 텍스처 로드
    bool LoadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font);
    //이미지 텍스처 로드
    bool LoadFromFile(std::string path);

    //렌더링
    void Render(float x, float y, SDL_FRect *clip = nullptr,
        float width = kOriginalSize, float height = kOriginalSize,
        double degrees = 0.0, SDL_FPoint* center = nullptr, SDL_FlipMode flipmode = SDL_FLIP_NONE
    );

    //텍스처 속성 가져오기
    int GetWidth();
    int GetHeight();
    bool isLoaded();

    SDL_Texture* mTexture;

    //텍스처 크기
    int mWidth;
    int mHeight;
    private:
};

class TextTexture : public Texture {
    public:

    std::vector<float> mTextLengthVector;
};