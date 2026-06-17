#include "pch.h"
#include "ui.h"

#include "system.h"
#include "game_state.h"
#include "texture.h"
#include "square.h"

//무조건 이걸로만 생성해라..
UI::UI(Square* uiFrame, std::string uiText)
{
    mUIFrame = uiFrame;
    mUIText = uiText;
    mTexture = SDL_CreateTexture(
        System::sRenderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        System::sWindowWidth, System::sWindowHeight
    );
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

void UI::HandleEvent(SDL_Event &e, GameStateManager& gsm, float mouseX, float mouseY)
{

}

//이렇게 하면 성능 부하가 크다. 이 함수 이제 쓰지마라. 밑에거 써라.
//근데 해보니까 아니다. 밑에거 함수 너무 더러우니까 이걸로 쓰자..
void UI::RenderOnUpdate()
{
    //업데이트 플래그가 거짓이면 저장된 텍스처를 렌더링한다.
    if (mIsUIUpdate == false) {
        SDL_RenderTexture(System::sRenderer ,mTexture, nullptr, nullptr);
        return;
    }
    //업데이트 플래그가 참이면
    SDL_Log("updating ui on update flag");

    SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(mTexture, SDL_SCALEMODE_LINEAR);

    SDL_SetRenderTarget(System::sRenderer, mTexture); //렌더러 타겟으로 설정
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(System::sRenderer);

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
            textTexture.LoadFromRenderedText(mUIText.substr(i, 3), 0, textColor);
            i += 2;
        }
        else {
            textTexture.LoadFromRenderedText(mUIText.substr(i, 1), 0, textColor);
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

    SDL_SetRenderTarget(System::sRenderer, NULL); //렌더러 타겟에서 해제
    mIsUIUpdate = false; //렌더링 완료하면 플래그 변수 초기화
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

        //텍스트가 프레임 y축을 벗어나는 것을 확인
        if (accumulatedHeight > mUIFrame->GetH() - mPadding*2) {
            complete = true;
            break;
        }

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

Button::Button(Square *uiFrame, std::string uiText, BtnType type)
{
    mUIFrame = uiFrame;
    mUIText = uiText;
    mType = type;

    mTexture = SDL_CreateTexture(
        System::sRenderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        System::sWindowWidth, System::sWindowHeight
    );
}

void Button::HandleEvent(SDL_Event &e, GameStateManager& gsm, float mouseX, float mouseY)
{
    //버튼 안에 있는지 확인
    if (e.button.x < mUIFrame->GetX()) return;
    if (e.button.x > mUIFrame->GetX() + mUIFrame->GetW()) return;    
    if (e.button.y < mUIFrame->GetY()) return;
    if (e.button.y > mUIFrame->GetY() + mUIFrame->GetH()) return;    
    
    //여기에 마우스 오버 이벤트 로직을 입력.

    //클릭했는지 확인
    if (e.type != SDL_EVENT_MOUSE_BUTTON_DOWN) return;

    //버튼 타입에 따라 반응함. 헤으응.
    //시작 버튼을 눌렀을 경우
    if (mType == BtnType::Start) {
        SDL_Log("start button pressed");
        gsm.mNextState = gsm.mOms; //다음 타깃 상태는 오버맵 상태다.
        gsm.mIsStateChange = true;
    }
    else {
        SDL_Log("おはよう");
    }
}

void UIManager::RenderUIs()
{
    SDL_SetRenderLogicalPresentation(System::sRenderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);

    for (auto ui : uiMap) {
        ui.second->RenderOnUpdate();
    }
}

void UIManager::DestroyUIs()
{
    for (auto ui : uiMap) {
        //ui 텍스처 파괴 로직
        if (ui.second->mTexture != nullptr) {
            SDL_DestroyTexture(ui.second->mTexture);
        }

        //ui 자체 메모리 해제
        delete ui.second;
    }

    uiMap.clear();
}
