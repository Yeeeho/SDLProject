#include "pch.h"
#include "ui.h"

#include "texture.h"
#include "square.h"

//무조건 이걸로만 생성해라..
UI::UI(Square* uiFrame, std::string uiText)
{
    mUIFrame = uiFrame;
    mUIText = uiText;
}

void UI::Render()
{
    mUIFrame->Render();
    Texture textTexture;
    SDL_Color textColor {0x00, 0xE0, 0x00, 0xFF};

    int totalHeight = 0;
    float totalWidth = 0;
    for (int i = 0; i < mUIText.length(); i++) {

        //utf-8에서 한글은 한 문자가 3바이트다. 그걸 구분해야함.
        //3바이트 문자의 첫번째 바이트는 0b1110xxxx 이므로 비트 비교로 3바이트 문자 추출
        if ((mUIText[i] & 0b11110000) == 0b11100000) {
            //이새끼는 한글이구나
            textTexture.LoadFromRenderedText(mUIText.substr(i, 3), 20, textColor);
            i += 2;
        }
        else {
            textTexture.LoadFromRenderedText(mUIText.substr(i, 1), 20, textColor);
        }

        //텍스트 텍스처가 프레임 x축 범위를 넘어갔을 경우
        if (mUIFrame->GetX() + mPadding + totalWidth + textTexture.GetWidth() > mUIFrame->GetX() + mUIFrame->GetW() - mPadding) {
            totalHeight += textTexture.GetHeight();  //총높이에 현재 높이도 추가해준다.
            totalWidth = 0; //총넓이 초기화 잊지말자
        }
        
        //텍스트 텍스처가 프레임 y축 범위를 넘어갔을 경우
        if (mUIFrame->GetY() + mPadding + totalHeight + textTexture.GetHeight() > mUIFrame->GetX() + mUIFrame->GetW() - mPadding) {
            break; //이제 그만 안식에 든다.
        }
        
        //줄의 첫 시작이 공백이면 반복 한번 스킵한다.
        if (totalWidth == 0 && mUIText.substr(i, 1) == " ") {
            continue;
        }
          
        //렌더링
        //총 넓이가 얼마였는지 기억해서 더해준다.
        textTexture.Render(mUIFrame->GetX() + mPadding + totalWidth, mUIFrame->GetY()  + mPadding + totalHeight);
        //렌더링한 텍스처만큼 총 넓이 변수에 더해준다.
        totalWidth += static_cast<float>(textTexture.GetWidth());
    }
}

void UI::SetPadding(float padding)
{
    mPadding = padding;
}

float UI::GetPadding()
{
    return mPadding;
}

Button::Button(Square *uiFrame, std::string uiText)
{
    mUIFrame = uiFrame;
    mUIText = uiText;
}

void Button::HandleEvent(SDL_Event &e, float mouseX, float mouseY)
{
    if (!e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) return; //클릭 감지
    //버튼 안에 있는지 확인
    if (e.button.x < mUIFrame->GetX()) return;
    if (e.button.x > mUIFrame->GetX() + mUIFrame->GetW()) return;    
    if (e.button.y < mUIFrame->GetY()) return;
    if (e.button.y > mUIFrame->GetY() + mUIFrame->GetH()) return;    

    SDL_Log("おはよ。");
}

void UIManager::InitUIs()
{
    uiMap["testButton"] = new UI(new Square(10, 400, 100, 100), "ohayo gozaimasu");
}

void UIManager::RenderUIs()
{
    for (auto ui : uiMap) {
        ui.second->Render();
    }
}
