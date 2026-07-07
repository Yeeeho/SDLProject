#include "pch.h"
#include "ui.h"

#include "system.h"
#include "util.h"
#include "map.h"
#include "game_object.h"
#include "game_state.h"
#include "text.h"
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

UIManager::UIManager()
{
    mToolTip = new ToolTip();
}

void UIManager::RenderUIs()
{
    SDL_SetRenderLogicalPresentation(System::sRenderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);

    //기본 ui 렌더링
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

void UIManager::LoadMapToolTip(const ObjectManager& objm, int tileId)
{
    TextUI* tui = mToolTip->mTui;

    SDL_Color tc {0x00, 0xB0, 0x00, 0xff};
    tui->mTexts.push_back(TTFWord("타일 id:", tc, System::sFont));
    tui->mTexts.push_back(TTFWord(System::sFont, TextType::Space));
    tui->mTexts.push_back(TTFWord(std::to_string(tileId), tc, System::sFont));
    tui->mTexts.push_back(TTFWord(System::sFont, TextType::NewLine));
    
    if (objm.mMap->mMapTiles[tileId]) {

    }
}

void UIManager::UpdateMapToolTip(const ObjectManager &objm)
{
    mToolTip->CheckUpdate();

    // 툴팁 내부 텍스트, 툴팁이 업데이트 되었을때 로드
    if (mToolTip->mIsUIUpdate) {
        TextUI* tui = mToolTip->mTui;
        tui->mIsUIUpdate = true;

        tui->mTexts.clear();
        tui->mTotalWidth = 0; tui->mTotalHeight = 0;

        int id = WhatTileOnPoint(mToolTip->mRefX, mToolTip->mRefY, objm.mMap->mX, objm.mMap->mY,
            objm.mMap->mXTiles, objm.mMap->mYTiles, objm.mMap->mTileLen, objm.mMap->mTileLen
        );

        LoadMapToolTip(objm, id);
    }
}

void UIManager::HandleMapToolTipEvent(SDL_Event &e, GameStateManager &gsm, ObjectManager &objm, float mouseX, float mouseY)
{
   //마우스가 맵 안에 있는지 확인
    bool mouseIn = MouseCollisionCheck(mouseX, mouseY, 
        static_cast<float>(objm.mMap->mX), static_cast<float>(objm.mMap->mY),
        static_cast<float>(objm.mMap->mW), static_cast<float>(objm.mMap->mH)
    );
    //마우스가 맵 안에 있으면 렌더링함.
    if (mouseIn) mToolTip->mIsRender = true;
    else mToolTip->mIsRender = false;

    //mouseover 중인 타일의 id를 구함
    int id = WhatTileOnPoint(mouseX, mouseY, objm.mMap->mX, objm.mMap->mY,
        objm.mMap->mXTiles, objm.mMap->mYTiles, objm.mMap->mTileLen, objm.mMap->mTileLen
    );
    MapTile* tile = objm.mMap->mMapTiles[id];
    //타일 좌표를 툴팁의 참조 좌표에 할당해줌
    mToolTip->SetRefInfo(tile->mX, tile->mY, tile->mW, tile->mH);
    //툴팁 이벤트 핸들링
    mToolTip->HandleEvent(e, gsm, mouseX, mouseY);
}

bool UIManager::MouseCollisionCheck(float mouseX, float mouseY, float mX, float mY, float mW, float mH)
{
    bool mouseIn = false;

    if (mouseX < mX) return mouseIn;
    if (mouseX > mX + mW) return mouseIn;
    if (mouseY < mY) return mouseIn;
    if (mouseY > mY + mH) return mouseIn;

    //마우스가 안에 있음
    mouseIn = true;
    return mouseIn;
}

int UIManager::WhatTileOnPoint(float mouseX, float mouseY, int mX, int mY, int xTiles, int yTiles, int xTileLen, int yTileLen)
{
    float xDis = mouseX - static_cast<float>(mX);
    float yDis = mouseY - static_cast<float>(mY);
    int xPos = xDis/xTileLen; //x축 타일 좌표 검사. 첫번째는 0
    int yPos = yDis/yTileLen; //y축도 동일

    //타일 좌표 클램핑
    if (xPos >= xTiles) xPos = xTiles - 1; 
    if (yPos >= yTiles) yPos = yTiles - 1;
    if (xPos < 0) xPos = 0;
    if (yPos < 0) yPos = 0;

    //맵 아이디를 구한다.
    int id = xPos + (xTiles * yPos);

    return id;
}

ToolTip::ToolTip()
{
    mUIFrame = new Square(mX, mY, 100, 100);

    mTui = new TextUI(0.f, 0.f);

    TextureManager tm;
    mTexture = tm.CreateTempTexture();
}

void ToolTip::Destroy()
{
    //ui 텍스처 파괴 로직
    if (mTexture != nullptr) {
        SDL_DestroyTexture(mTexture); //임시 텍스처 해제
    }
    if (mMyTexture != nullptr) {
        mMyTexture->Destroy();
        delete mMyTexture; //현재 텍스처 해제
    }
    if (mTui != nullptr) delete mTui;

    delete this; //본인 해제
}

void ToolTip::SetToolTipFrame()
{
    float currentW = 0.f, currentH = 0.f;
    float maxW = 0.f, maxH = 0.f;
    bool firstWord = true; //첫 단어에서 사용되고 거짓으로 바뀜

    for (TTFWord word : mTui->mTexts) {
        //폰트 높이 캐싱
        int fontH = TTF_GetFontHeight(word.mFont);
        //단어가 있으면 높이 추가
        if (firstWord) {
            maxH += fontH;
            firstWord = false;
        }

        //띄어쓰기일때
        if (word.mType == TextType::Space) {
            currentW += fontH * 0.5; 
        }
        //줄바꿈일때
        if (word.mType == TextType::NewLine) {

            if (maxW <= currentW) maxW = currentW; //최대값 캐싱
            currentW = 0; //초기화
            maxH += fontH;
        }
        //문자일때
        else {
            for (int i = 0; i < word.mMessage.length(); i++) {
                //언어 감지
                if ((word.mMessage[i] & 0b11110000) == 0b11100000) {
                    //한글
                    currentW += fontH; //한글이니까 그대로
                    i += 2;
                }
                else{
                    currentW += fontH * 0.5;
                } 
            }
        }
    }
    if (maxW <= currentW) maxW = currentW; //최대값 캐싱
    currentW = 0; //초기화

    SDL_Log(std::to_string(maxW).c_str());
    mUIFrame->SetW(static_cast<int>(maxW));
    mUIFrame->SetH(static_cast<int>(maxH));
}

void ToolTip::SetRefInfo(int x, int y, int w, int h)
{
    mRefX = x; mRefY = y;
    mRefW = w; mRefH = h;
}

void ToolTip::CheckUpdate()
{
    //저장된 좌표 정보가 현재 참조 좌표 정보와 일치함
    if (mPrevX == mRefX && mPrevY == mRefY) {
        mIsUIUpdate = false;
    }
    //새로운 좌표로 이동했음
    else {
        mIsUIUpdate = true;
        SDL_Log("upate tooltip");
        mX = mRefX + mRefW * 0.5; //가운데쯤에 생성
        mY = mRefY + mRefH * 0.5;
        
        mPrevX = mRefX; mPrevY = mRefY; //좌표 정보 다시 캐싱
        mPrevW = mRefW; mPrevH = mRefH; 
    }
}

void ToolTip::HandleEvent(SDL_Event &e, GameStateManager &gs, float mouseX, float mouseY)
{
    //마우스가 참조 객체 좌표 안에 있는지 확인
    //마우스가 프레임 밖에 있는가?
    if (mouseX < mRefX || mouseX > mRefX + mRefW    
        || mouseY < mRefY || mouseY > mRefY + mRefH) {
            return;
    }
    //마우스가 프레임 안에 있을때

    //클릭했는지 확인
    if (e.type != SDL_EVENT_MOUSE_BUTTON_DOWN) return;
}

void ToolTip::RenderOnUpdate()
{
    //렌더링 할지말지 정한다.
    if (mIsRender == false) {
        return;
    }

    //업데이트 플래그가 거짓이면 저장된 텍스처를 렌더링한다.
    if (mIsUIUpdate == false) {
        //위치 정보에 따라 사각형 생성
        SDL_FRect fr = {mX, mY, static_cast<float>(System::sWindowWidth), static_cast<float>(System::sWindowHeight)};
        SDL_RenderTexture(System::sRenderer ,mTexture, nullptr, &fr);
        return;
    }
    //업데이트 플래그가 참이면

    //렌더러 타겟팅
    SDL_SetRenderTarget(System::sRenderer, mTexture);
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
    SDL_RenderClear(System::sRenderer);

    //실제 로직
    mUIFrame->SetX(0.f); mUIFrame->SetY(0.f); //위치 설정
    SetToolTipFrame(); //동적 크기 설정
    mTui->mX = 0.f; mTui->mY = 0.f;

    mUIFrame->Render(0x00, 0xB0, 0x00, 0xFF, 0x08, 0x08, 0x08, 0xD0);
    mTui->RenderWords();

    //렌더러 타겟 해제
    SDL_SetRenderTarget(System::sRenderer, NULL);
    
    //렌더링 완료하면 플래그 변수 초기화
    mIsUIUpdate = false;
}

//실시간 렌더링 함수. 지금은 쓰지 않음.
void ToolTip::Render()
{
    if (mIsRender == false) {
        return;
    }

    mUIFrame->SetX(mX); mUIFrame->SetY(mY);
    mUIFrame->Render();
}

TextUI::TextUI(float x, float y)
{
    mX = x; mY = y;

    TextureManager tm;
    mTexture = tm.CreateTempTexture();
}

void TextUI::RenderWords()
{
    for (TTFWord word : mTexts) {
        if (word.mType == TextType::NewLine) NewLine(word.mFont);
        else if (word.mType == TextType::Space) AddSpace(word.mFont);
        else RenderAtLine(word);
    }
}

void TextUI::RenderAtLine(const TTFWord &word)
{
    Texture textTexture; //렌더링할 텍스처

    std::string message = word.mMessage;
    TTF_Font* font = word.mFont;
    SDL_Color color = word.mColor;

    for (int i = 0; i < message.length(); i++) {

        if ((message[i] & 0b11110000) == 0b11100000) {
            //이새끼는 한글이구나
            textTexture.LoadFromRenderedText(message.substr(i, 3), color, font);
            i += 2;
        }
        else{
            textTexture.LoadFromRenderedText(message.substr(i, 1), color, font);
        } 

        //렌더링
        //총 넓이가 얼마였는지 기억해서 더해준다.
        textTexture.Render(mX + mPadding + mTotalWidth, mY + mPadding + mTotalHeight);
        //렌더링한 텍스처만큼 총 넓이 변수에 더해준다.
        mTotalWidth += textTexture.GetWidth();
    }
}

void TextUI::NewLine(TTF_Font* font)
{
    mTotalWidth = 0; //총 길이 초기화
    mTotalHeight += TTF_GetFontHeight(font);
}

void TextUI::AddSpace(TTF_Font *font)
{
    mTotalWidth += TTF_GetFontHeight(font)/2;
}

void TextUI::RenderOnUpdate()
{
    //업데이트 플래그가 거짓이면 저장된 텍스처를 렌더링한다.
    if (mIsUIUpdate == false) {
        SDL_RenderTexture(System::sRenderer ,mTexture, nullptr, nullptr);
        return;
    }
    //업데이트 플래그가 참이면
    SDL_Log("updating text ui on update flag");

    //렌더러 타겟팅
    SDL_SetRenderTarget(System::sRenderer, mTexture);
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_SetTextureBlendMode(mTexture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
    SDL_RenderClear(System::sRenderer);

    //렌더링 실제 동작
    RenderWords();

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
