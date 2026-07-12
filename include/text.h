#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>

enum class TextType {
    Default, NewLine, Space
};

class TTFWord {
    public:
    TTFWord(TTF_Font* font, TextType type);
    TTFWord(std::string message, SDL_Color color, TTF_Font* font);

    void Destroy();

    int GetWordWidth();
    int GetWordHeight();

    std::string mMessage {""};
    SDL_Color mColor {0x00, 0x00, 0x00, 0x00};
    TTF_Font* mFont {nullptr};

    TextType mType {TextType::Default};
};