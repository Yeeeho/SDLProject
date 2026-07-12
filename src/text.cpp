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

void TTFWord::Destroy()
{
    if (this != nullptr) {
        delete this;
    }
}

int TTFWord::GetWordWidth()
{
    //폰트 높이 캐싱
    int fontH = TTF_GetFontHeight(mFont);

    //디폴트 타입이 아닌 다른 타입일때
    if (mType == TextType::Space) {
        int ret = fontH * 0.5;
        return ret;
    }
    else if (mType == TextType::NewLine) {
        return 0;
    }

    //디폴트 타입일때
    //길이 구함
    int ttWidth = 0; //누적 길이
    for (int i = 0; i < mMessage.length(); i++) {
        //언어 감지
        if ((mMessage[i] & 0b11110000) == 0b11100000) {
            //한글
            ttWidth += fontH; //한글이니까 그대로
            i += 2;
        }
        else{
            ttWidth += fontH * 0.5;
        } 
    }

    return ttWidth;
}

int TTFWord::GetWordHeight()
{
    int ret = TTF_GetFontHeight(mFont);
    return ret;
}
