#pragma once

#include <string>

#include "texture.h"

class TextTexture {
    public:
    Texture* mTextTexture{nullptr};

    bool LoadFromRenderedText(std::string text, SDL_Color textColor);
};