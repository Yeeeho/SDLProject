#include "pch.h"

#include "text.h"

TTFWord::TTFWord(TTF_Font *font, TextType type)
{
    //기본값으로 설정
    mMessage = "null";
    mColor = SDL_Color();
    //매개변수에서 할당
    mFont = font;
    mType = type;
}

TTFWord::TTFWord(std::string message, SDL_Color color, TTF_Font *font)
{
    mMessage = message;
    mColor = color;
    mFont = font;
    //기본값으로 설정
    mType = TextType::Default;
}
