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

float UI::ClipDesiredTextLength(TextTexture& ttexture)
{
    //실제 길이제한
    float rawW = mUIFrame->GetW() - mPadding*4;

    //누적된 길이
    float accumulatedW = 0.f;
    for (auto textinf = ttexture.mTextLengthVector.begin(); textinf != ttexture.mTextLengthVector.end();) {
        
        //텍스트 길이가 실제 제한을 벗어나면
        if (accumulatedW > rawW) {
            //길이 덧셈을 취소후 반환
            accumulatedW -= *textinf;
            break;
        }

        accumulatedW += *textinf;

        //현재 요소를 맵에서 삭제함
        //erase 함수는 다음 반복자를 반환하므로 내가 증감해줄 필요는 없다.
        textinf = ttexture.mTextLengthVector.erase(textinf);
    }

    return accumulatedW;
}

void UI::HandleEvent(SDL_Event &e, float mouseX, float mouseY)
{
    if (e.key.key == SDLK_1 && e.type == SDL_EVENT_KEY_DOWN) {
        SDL_Log("dddddd");
    }
}

//이렇게 하면 성능 부하가 크다. 이 함수 이제 쓰지마라. 밑에거 써라.
void UI::Render(bool& isUIUpdate)
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

    isUIUpdate = false; //렌더링 완료하면 업데이트 완료 상태로 바꿈
}

void UI::Render()
{
    mUIFrame->Render();
    TextTexture textTexture;
    SDL_Color textColor {0x00, 0xE0, 0x00, 0xFF};

    //줄바꾸는 로직을 더 성능 부하 적은 방법으로 만든다.
    //이친구를 사실상 스프라이트 시트로 쓰는거다..
    textTexture.LoadFromRenderedText(mUIText, 20, textColor);

    //총 길이, 누적 길이를 기억해놓는다.
    float totalWidth = textTexture.GetWidth();
    float accumulatedWidth = 0.f;
    float accumulatedHeight = 0.f;
    //클립이 사용할 넓이값
    float clipW = 0.f;

    //텍스처가 프레임의 x축 한계선에 도달했을 경우 잘라내서 렌더링한다.
    bool complete = false;
    while (!complete) {

        float tempBoi = ClipDesiredTextLength(textTexture);

        //남은 길이가 프레임 내부 x축제한보다 작을 경우
        if (totalWidth < mUIFrame->GetW() - mPadding*2) {
            clipW = totalWidth;
            complete = true;
        }
        else {
            clipW = tempBoi;
        }

        SDL_FRect clip {
            accumulatedWidth, 0.f,
            clipW, (float)textTexture.GetHeight()
        };
        
        textTexture.Render(
            mUIFrame->GetX() + mPadding, 
            mUIFrame->GetY() + mPadding + accumulatedHeight, &clip
        );
                
        //증감할거 증감해준다.
        totalWidth -= tempBoi;
        accumulatedHeight += textTexture.GetHeight();
        accumulatedWidth += tempBoi;
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
    if (e.type != SDL_EVENT_MOUSE_BUTTON_DOWN) return;
    //버튼 안에 있는지 확인
    if (e.button.x < mUIFrame->GetX()) return;
    if (e.button.x > mUIFrame->GetX() + mUIFrame->GetW()) return;    
    if (e.button.y < mUIFrame->GetY()) return;
    if (e.button.y > mUIFrame->GetY() + mUIFrame->GetH()) return;    

    SDL_Log("おはよう");
}

void UIManager::InitUIs()
{
    uiMap["testButton"] = new Button(new Square(10, 400, 100, 100), "ohayo gozaimasu");
}

void UIManager::RenderUIs()
{
    for (auto ui : uiMap) {
        ui.second->Render();
    }
}
