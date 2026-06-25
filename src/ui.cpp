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

    TextureManager tm;
    mTexture = tm.CreateTempTexture();
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

    SDL_SetRenderTarget(System::sRenderer, mTexture); //렌더러 타겟으로 설정
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND_PREMULTIPLIED); //알파 이중 합성이 일어나지 않게 모드를 정한다.
    SDL_RenderClear(System::sRenderer);

    mUIFrame->Render();
    Texture textTexture;
    SDL_Color textColor {0x00, 0xE0, 0x00, 0xFF};

    int totalHeight = 0;
    int totalWidth = 0;
    for (int i = 0; i < mUIText.length(); i++) {

        //utf-8에서 한글은 한 문자가 3바이트다. 그걸 구분해야함.
        //3바이트 문자의 첫번째 바이트는 0b1110xxxx 이므로 비트 비교로 3바이트 문자 추출
        if ((mUIText[i] & 0b11110000) == 0b11100000) {
            //이새끼는 한글이구나
            textTexture.LoadFromRenderedText(mUIText.substr(i, 3), textColor, System::sFont);
            i += 2;
        }
        else {
            textTexture.LoadFromRenderedText(mUIText.substr(i, 1), textColor, System::sFont);
        }

        //텍스트 텍스처가 프레임 x축 범위를 넘어갔을 경우
        if (mUIFrame->GetX() + mPadding + totalWidth + textTexture.GetWidth() > mUIFrame->GetX() + mUIFrame->GetW() - mPadding) {
            totalHeight += textTexture.GetHeight();  //총높이에 현재 높이도 추가해준다.
            totalWidth = 0; //총넓이 초기화 잊지말자
        }
        
        //텍스트 텍스처가 프레임 y축 범위를 넘어갔을 경우
        if (mUIFrame->GetY() + mPadding + totalHeight + textTexture.GetHeight() > mUIFrame->GetY() + mUIFrame->GetH() - mPadding) {
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

//이제 안쓴다. 이런게 있었다는 것을 기억해둬라.
void UI::Render()
{
    mUIFrame->Render();
    Texture textTexture;
    SDL_Color textColor {0x00, 0xE0, 0x00, 0xFF};

    //줄바꾸는 로직을 더 성능 부하 적은 방법으로 만든다.
    //이친구를 사실상 스프라이트 시트로 쓰는거다..
    textTexture.LoadFromRenderedText(mUIText, textColor, System::sFont);

    //총 길이, 누적 길이를 기억해놓는다.
    float totalWidth = textTexture.GetWidth();
    float accumulatedWidth = 0.f;
    float accumulatedHeight = 0.f;
    //클립이 사용할 넓이값
    float clipW = 0.f;

    //텍스처가 프레임의 x축 한계선에 도달했을 경우 잘라내서 렌더링한다.
    bool complete = false;
    while (!complete) {

        float tempBoi = 1.5f; //디버그

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

    TextureManager tm;
    mTexture = tm.CreateTempTexture();
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
    //버튼을 눌렀을 경우
    if (mType == BtnType::OverMap) {
        SDL_Log("overmap button pressed");
        gsm.mNextState = gsm.mOms; //다음 타깃 상태는 오버맵 상태다.
        gsm.mIsStateChange = true;
    }
    else if (mType == BtnType::City) {
        SDL_Log("city button pressed");
        gsm.mNextState = gsm.mCvs;
        gsm.mIsStateChange = true;
    }
    else if (mType == BtnType::Title) {
        SDL_Log("title button pressed");
        gsm.mNextState = gsm.mIs; //다음 타깃 상태는 인트로다.
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
        if (ui.second->mMyTexture != nullptr) {
            ui.second->mMyTexture->Destroy();
            delete ui.second->mMyTexture;
        }
        //ui 자체 메모리 해제
        delete ui.second;
        ui.second = nullptr;
    }

    uiMap.clear();
}

MaptileUI::MaptileUI()
{

}

void MaptileUI::HandleEvent(SDL_Event &e, GameStateManager &gs, float mouseX, float mouseY)
{
    //ui 안에 있는지 확인
    if (e.button.x < mUIFrame->GetX()) return;
    if (e.button.x > mUIFrame->GetX() + mUIFrame->GetW()) return;    
    if (e.button.y < mUIFrame->GetY()) return;
    if (e.button.y > mUIFrame->GetY() + mUIFrame->GetH()) return;    
    
    //여기에 마우스 오버 이벤트 로직을 입력.

    //클릭했는지 확인
    if (e.type != SDL_EVENT_MOUSE_BUTTON_DOWN) return;
}

TextUI::TextUI(int x, int y, int width, int height, std::string uiText)
{
    mUIText = uiText;
    mX = x; mY = y; mW = width; mH = height;

    TextureManager tm;
    mTexture = tm.CreateTempTexture();
}

TextUI::TextUI(int x, int y, TTF_Font *font, std::string text)
{
    mUIText = text;
    mX = x; mY = y;

    mFont = font;

    TextureManager tm;
    mTexture = tm.CreateTempTexture();
}

void TextUI::RenderOnUpdate()
{
    //업데이트 플래그가 거짓이면 저장된 텍스처를 렌더링한다.
    if (mIsUIUpdate == false) {
        SDL_RenderTexture(System::sRenderer ,mTexture, nullptr, nullptr);
        return;
    }
    //업데이트 플래그가 참이면
    SDL_Log("updating ui on update flag");

    //렌더러 타겟팅
    SDL_SetRenderTarget(System::sRenderer, mTexture);
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
    SDL_RenderClear(System::sRenderer);

    //렌더링 실제 동작
    Texture textTexture;
    SDL_Color textColor {0x00, 0xE0, 0x00, 0xFF};

    int totalHeight = 0;
    int totalWidth = 0;
    for (int i = 0; i < mUIText.length(); i++) {

        //utf-8에서 한글은 한 문자가 3바이트다. 그걸 구분해야함.
        //3바이트 문자의 첫번째 바이트는 0b1110xxxx 이므로 비트 비교로 3바이트 문자 추출
        if ((mUIText[i] & 0b11110000) == 0b11100000) {
            //이새끼는 한글이구나
            textTexture.LoadFromRenderedText(mUIText.substr(i, 3), textColor, mFont);
            i += 2;
        }
        else {
            textTexture.LoadFromRenderedText(mUIText.substr(i, 1), textColor, mFont);
        }

        //렌더링
        //총 넓이가 얼마였는지 기억해서 더해준다.
        textTexture.Render(mX + mPadding + totalWidth, mY + mPadding + totalHeight);
        //렌더링한 텍스처만큼 총 넓이 변수에 더해준다.
        totalWidth += textTexture.GetWidth();
    }

    //렌더러 타겟 해제
    SDL_SetRenderTarget(System::sRenderer, NULL);
    
    //렌더링 완료하면 플래그 변수 초기화
    mIsUIUpdate = false;
}

IconUI::IconUI(int x, int y, int width, int height, std::string path)
{
    mX = x; mY = y; mW = width; mH = height;
    mMyTexture = new Texture();
    if (mMyTexture->LoadFromFile(path) == false) {
        SDL_Log("could not load icon");
    }
    
    SDL_SetTextureBlendMode(mMyTexture->mTexture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
}

void IconUI::RenderOnUpdate()
{
    mMyTexture->Render(mX, mY, nullptr, mW, mH);
}

void IconUI::HandleEvent(SDL_Event &e, GameStateManager &gs, float mouseX, float mouseY)
{
    //버튼 안에 있는지 확인
    if (e.button.x < mX) return;
    if (e.button.x > mX + mW) return;    
    if (e.button.y < mY) return;
    if (e.button.y > mY + mH) return;    
    
    //여기에 마우스 오버 이벤트 로직을 입력.

    //클릭했는지 확인
    if (e.type != SDL_EVENT_MOUSE_BUTTON_DOWN) return;
}
