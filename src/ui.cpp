#include "pch.h"
#include "ui.h"

#include "system.h"
#include "scenario.h"
#include "physics.h"
#include "camera.h"
#include "render.h"
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
    mTempTex = tm.CreateTempTexture();
}

void UI::Destroy()
{
    if (mUIFrame != nullptr) delete mUIFrame;
    if (mTempTex != nullptr) SDL_DestroyTexture(mTempTex);
    if (mMyTexture != nullptr) mMyTexture->Destroy();

    delete this;
}

void UI::HandleEvent(SDL_Event &e, GameStateManager& gsm, ObjectManager& objm, float mouseX, float mouseY)
{

}

//이렇게 하면 성능 부하가 크다. 이 함수 이제 쓰지마라. 밑에거 써라.
//근데 해보니까 아니다. 밑에거 함수 너무 더러우니까 이걸로 쓰자..
void UI::RenderOnUpdate()
{
    //업데이트 플래그가 거짓이면 저장된 텍스처를 렌더링한다.
    if (mIsUIUpdate == false) {
        SDL_RenderTexture(System::sRenderer ,mTempTex, nullptr, nullptr);
        return;
    }
    //업데이트 플래그가 참이면
    SDL_Log("updating ui on update flag");

    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

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

void UI::SetFrameColor(SDL_Color fillcolor, SDL_Color linecolor)
{
    mUIFrame->SetColor(fillcolor, linecolor);
}

Button::Button(Square *uiFrame, std::string uiText, BtnType type)
{
    mUIFrame = uiFrame;
    mUIText = uiText;
    mType = type;

    TextureManager tm;
    mTempTex = tm.CreateTempTexture();
}

void Button::HandleEvent(SDL_Event &e, GameStateManager& gsm, ObjectManager& objm, float mouseX, float mouseY)
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
    else if (mType == BtnType::SubMap) {
        SDL_Log("change to submap state");
        gsm.mNextState = gsm.mSms;
        gsm.mIsStateChange = true;
    }
    else if (mType == BtnType::NewGame) {
        SDL_Log("new game start");
        gsm.mNextState = gsm.mSms;
        gsm.mScm->SetCurrentScenario(new NGScenario(), objm);
        gsm.mIsStateChange = true;
    }
    else {
        SDL_Log("button action not specified");
    }
}

UIManager::UIManager()
{
    int panelX = System::sWindowWidth * 0.5 - 400;
    int panelY = System::sWindowHeight - 300;

    mToolTip = new ToolTip();
    mDialogueUI = new DialogueUI((float) panelX,(float) panelY);
}

void UIManager::InitTopBar()
{
    //탑 바 패널 생성
    mPanels["topPanel"] = new Square(0, 0, System::sWindowWidth, mTopPanelH);

    //ui 객체들 생성
    uiMap["turnIcon"] = new IconUI(10, 0, 60, 60, "images/icons/turn.png");

    TextUI* turnTui = new TextUI(70, 0);
    SDL_Color tc = {0x00, 0xD0, 0x00, 0xFF};
    turnTui->mTexts.push_back(TTFWord("0", tc, System::sFont40));

    uiMap["turnText"] = turnTui;

    uiMap["supplyIcon"] = new IconUI(130, 0, 60, 60, "images/icons/supply.png");
    uiMap["supplyText"] = new TextUI(190, 0);
}

void UIManager::InitUIs()
{
}

void UIManager::HandleUIEvent(SDL_Event &e, GameStateManager &gsm, ObjectManager& objm, float mouseX, float mouseY)
{
    for (auto ui : uiMap) {
        ui.second->HandleEvent(e, gsm, objm, mouseX, mouseY);
    }
    for (auto ftui : ftuiMap) {
        ftui.second->HandleEvent(e, gsm, objm, mouseX, mouseY);
    }
}

void UIManager::RenderUIs()
{
    SDL_SetRenderLogicalPresentation(System::sRenderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);

    SDL_Color fillColor = {0x08, 0x08, 0x08, 0xF0};
    SDL_Color lineColor = {0x00, 0x80, 0x00, 0xFF};

    //ui 패널 렌더링
    for (auto panel : mPanels) {
        panel.second->Render(lineColor, fillColor);
    }
    
    for (auto ftui : ftuiMap) {
        ftui.second->RenderOnUpdate();
    }

    //기본 ui 렌더링
    for (auto ui : uiMap) {
        ui.second->RenderOnUpdate();
    }

}

void UIManager::RenderMapToolTip(Map *map)
{ 
    //카메라 오프셋
    mToolTip->mX = mToolTip->mRefX + mToolTip->mRefW * 0.5 - map->mCam->mSight.x;
    mToolTip->mY = mToolTip->mRefY + mToolTip->mRefH * 0.5 - map->mCam->mSight.y;

    mToolTip->RenderOnUpdate();
}

void UIManager::DestroyUIs()
{
    for (auto ui : uiMap) {
        //ui 텍스처 파괴 로직
        if (ui.second->mTempTex != nullptr) {
            SDL_DestroyTexture(ui.second->mTempTex);
        }
        if (ui.second->mMyTexture != nullptr) {
            ui.second->mMyTexture->Destroy();
            delete ui.second->mMyTexture;
        }
        //ui 자체 메모리 해제
        delete ui.second;
        ui.second = nullptr;
    }

    for (auto ftui : ftuiMap) {
        ftui.second->Destroy();
        ftui.second = nullptr;
    }
    for (auto square : mPanels) {
        delete square.second;
        square.second = nullptr;
    }

    uiMap.clear();
    ftuiMap.clear();
    mPanels.clear();
}

void UIManager::LoadMapToolTip(Map* map, int tileId)
{
    TextUI* tui = mToolTip->mTui;

    SDL_Color tc {0x00, 0xB0, 0x00, 0xFF};
    tui->mTexts.push_back(TTFWord("타일 id:", tc, System::sFont));
    tui->mTexts.push_back(TTFWord(System::sFont, TextType::Space));
    tui->mTexts.push_back(TTFWord(std::to_string(tileId), tc, System::sFont));
    tui->mTexts.push_back(TTFWord(System::sFont, TextType::NewLine));

    //타일 객체를 구함
    MapTile* tile = map->mMapTiles[tileId];
    for (TTFWord* word : tile->mInfos) {
        tui->mTexts.push_back(*word);
    }
}

void UIManager::UpdateMapToolTip(Map* map)
{
    mToolTip->CheckUpdate();

    // 툴팁 내부 텍스트, 툴팁이 업데이트 되었을때 로드
    if (mToolTip->mIsUIUpdate) {

        TextUI* tui = mToolTip->mTui;
        tui->mIsUIUpdate = true;

        tui->mTexts.clear();
        tui->mTotalWidth = 0; tui->mTotalHeight = 0;

        MapManager mm;
        int id = mm.WhatTileOnPoint(mToolTip->mRefX, mToolTip->mRefY, map);

        LoadMapToolTip(map, id);

        tui->mIsUIUpdate = false; //플래그 초기화
    }
}

void UIManager::HandleMapToolTipEvent(SDL_Event &e, GameStateManager &gsm, Map* map, float mouseX, float mouseY)
{
    //카메라 때문에 생긴 오차 보정
    mouseX += map->mCam->mSight.x;
    mouseY += map->mCam->mSight.y;

    //마우스가 맵 안에 있는지 확인
    Physics phs;
    bool mouseIn = phs.IsPointInSquare(mouseX, mouseY, 
        static_cast<float>(map->mX), static_cast<float>(map->mY),
        static_cast<float>(map->mW), static_cast<float>(map->mH)
    );
    //마우스가 맵 안에 있으면 렌더링함.
    if (mouseIn) {
        mToolTip->mIsRender = true;
        
        if (!mWasMouseOnMap) {
            SDL_Log("mouse in map");
            mToolTip->mIsUIUpdate = true;
            mWasMouseOnMap = true;
        }
    }
    else {
        mToolTip->mIsRender = false;
        mWasMouseOnMap = false;
    } 

    //mouseover 중인 타일의 id를 구함
    MapManager mm;
    int id = mm.WhatTileOnPoint(mouseX, mouseY, map);

    MapTile* tile = map->mMapTiles[id];
    //타일 좌표를 툴팁의 참조 좌표에 할당해줌
    mToolTip->SetRefInfo(tile->mX, tile->mY, tile->mW, tile->mH);
    //툴팁 이벤트 핸들링
    mToolTip->HandleEvent(e, gsm, mouseX, mouseY);
}

ToolTip::ToolTip()
{
    mUIFrame = new Square(mX, mY, 100, 100);

    mTui = new TextUI(0.f, 0.f);

    TextureManager tm;
    mTempTex = tm.CreateTempTexture();
}

//ui 텍스처 파괴 로직
void ToolTip::Destroy()
{
    if (mTui != nullptr) delete mTui;

    Destroy(); //부모 클래스 파괴 메서드, 본인도 파괴함
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
            maxH += mTui->mLineSpacing;
        }
        //문자일때
        else {
            currentW += word.GetWordWidth();
        }
    }
    if (maxW <= currentW) maxW = currentW; //최대값 캐싱
    maxW += mPadding; //패딩 추가
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
    //이미 업데이트 플래그가 참이면 검사하지 않는다.
    if (mIsUIUpdate) {
        return;
    }

    //저장된 좌표 정보가 현재 참조 좌표 정보와 일치함
    if (mPrevX == mRefX && mPrevY == mRefY) {

    }
    //새로운 좌표로 이동했음
    else {
        mIsUIUpdate = true;
        SDL_Log("update tooltip");
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
        SDL_RenderTexture(System::sRenderer ,mTempTex, nullptr, &fr);
        return;
    }
    //업데이트 플래그가 참이면
    SDL_Log("tooltip render update");

    //렌더러 타겟팅
    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

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
    mTempTex = tm.CreateTempTexture();
}

void TextUI::ClearTexts()
{
    mTexts.clear();
    mTotalWidth = 0;
    mTotalHeight = 0;
}

void TextUI::ProcessAndAddText(std::string text, SDL_Color color, TTF_Font* font)
{
    std::string message = "";

    for (int i = 0; i < text.length(); i++) {
        //공백일 경우
        if (text.substr(i, 1) == " "){
            //텍스트를 푸시하고 공백도 푸시함
            mTexts.push_back(TTFWord(message, color, font));
            mTexts.push_back(TTFWord(font, TextType::Space));

            message = ""; //초기화
        }
        //한글 등 문자당 3바이트를 쓰는 언어일 경우
        else if ((text[i] & 0b11110000) == 0b11100000) {
            message += text.substr(i, 3);

            //문장 끝에 도달함
            if (i == text.length() - 3) {
                mTexts.push_back(TTFWord(message, color, font));
                return;
            }

            i += 2;
        }
        //문자당 1바이트인 언어일 경우
        else {
            message += text.substr(i, 1);

            //문장 끝에 도달함
            if (i == text.length() - 1) {
                mTexts.push_back(TTFWord(message, color, font));
                return;
            }
        }
    }
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
    mTotalHeight += mLineSpacing;
}

void TextUI::AddSpace(TTF_Font *font)
{
    mTotalWidth += TTF_GetFontHeight(font)/2;
}

void TextUI::RenderOnUpdate()
{
    //업데이트 플래그가 거짓이면 저장된 텍스처를 렌더링한다.
    if (mIsUIUpdate == false) {
        SDL_RenderTexture(System::sRenderer ,mTempTex, nullptr, nullptr);
        return;
    }
    //업데이트 플래그가 참이면
    SDL_Log("updating text ui on update flag");

    //렌더러 타겟팅
    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

    //렌더링 실제 동작
    RenderWords();

    //렌더러 타겟 해제
    SDL_SetRenderTarget(System::sRenderer, NULL);
    //렌더링 완료하면 플래그 변수 초기화
    mIsUIUpdate = false;
}


FramedTUI::FramedTUI(int x, int y, int w, int h)
{
    mX = x; mY = y; mW = w; mH = h;

    mUIFrame = new Square(x, y, w, h);
    mTui = new TextUI(x, y);

    TextureManager tm;
    mTempTex = tm.CreateTempTexture();
}

void FramedTUI::AddWord(TTFWord word)
{
    mTui->mTexts.push_back(word);
}

void FramedTUI::ClearText()
{
    mTui->mTexts.clear();
}

//공사중
void FramedTUI::RenderOnUpdate()
{
    if (mIsRender == false) return;

    if (mIsUIUpdate == false) {
        SDL_RenderTexture(System::sRenderer, mTempTex, nullptr, nullptr);
        return;
    }
    SDL_Log("ftui render update");

    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

    //실제 로직
    Render();

    //렌더러 타겟 해제 및 초기화
    SDL_SetRenderTarget(System::sRenderer, NULL);
    mIsUIUpdate = false;
}

//공사중
void FramedTUI::Render()
{
    //프레임에 맞춰서 줄바꿈

    int mTotalW = 0;
    int mTotalH = 0;
    for (TTFWord word: mTui->mTexts) {
        if (word.mType == TextType::NewLine) {
            mTotalW = 0;
            mTotalH += word.GetWordHeight();
            mTotalH += mTui->mLineSpacing;

            mTui->NewLine(word.mFont);
        }
        else if (word.mType == TextType::Space) {
            mTotalW += word.GetWordWidth();

            //넓이가 프레임을 벗어났는지 검사함, 넘어가면 줄바꿈
            if (mTotalW + mPadding * 2 > mW) {
                mTotalW = 0;
                mTui->NewLine(word.mFont);
            }
            else mTui->AddSpace(word.mFont); 
        }
        else {
            mTotalW += word.GetWordWidth();
            //넓이 검사
            if (mTotalW + mPadding * 2 > mW) {
                //줄바꿈후 원문 렌더링

                mTotalW = 0; //초기화 후 길이 더하기 
                mTotalW += word.GetWordWidth();
                mTui->NewLine(word.mFont);
                mTui->RenderAtLine(word);    
            }
            else mTui->RenderAtLine(word);    
        } 

        //ui 프레임을 벗어났는지 검사함.
        if (mTotalH + mPadding * 2 > mH) {
            SDL_Log("ftui hitting height limit boi");
            return;
        }
    }
}

//공사중
void FramedTUI::HandleEvent(SDL_Event &e, GameStateManager &gsm, ObjectManager& objm, float mouseX, float mouseY)
{
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

void IconUI::HandleEvent(SDL_Event &e, GameStateManager &gs, ObjectManager& objm, float mouseX, float mouseY)
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

DialogueUI::DialogueUI(float x, float y)
{
    mX = x; mY = y;

    TextureManager tm;
    mTempTex = tm.CreateTempTexture();
    mBasicTex = tm.CreateTempTexture();

    SDL_Color black = {0x00, 0x00, 0x00, 0xFF};

    //대화창을 초기화한다.
    mPanel = new Square(mX, mY, 800, 200);
    mSpeakerBg = new Texture("images/black.png");
    mSpeakerImg = new Texture("images/blank.png");
    mSpkrBlankImg = new Texture("images/blank.png");
    mSpeakerFrame = new Texture("images/ui/dialogue_pic_frame.png");
   
    mDialogueBodyBg = new Texture("images/black.png");
    mDialogueBody = new FramedTUI(mX + 200, mY + 20,  500, 160);
    mDialogueBody->SetFrameColor(black, black);
    mDialogueBody->mIsRender = true;

    mDialogueBodyFrame = new Texture("images/ui/dialogue_frame.png");

    StoreBasicTex();
}

void DialogueUI::HandleEvent(SDL_Event &e, GameStateManager &gsm, float mouseX, float mouseY)
{
    Physics ps;
    bool mouseIn = ps.IsPointInSquare(mouseX, mouseY,
         mPanel->GetX(), mPanel->GetY(), (float) mPanel->GetW(), (float) mPanel->GetH()
    );
    
    if (!mouseIn) return;
    //마우스 오버

    if (e.type != SDL_EVENT_MOUSE_BUTTON_DOWN) return;
    //클릭 동작

    SDL_Log("clicked dialogue panel");
    gsm.mScm->mCurrentSc->mIsDialogueUpdate = true;
    gsm.mScm->mCurrentSc->mIsUpdate = true;
}

void DialogueUI::Update(ScenarioManager &scm)
{
    if (!mIsUpdate) return;

    mIsUpdate = false;
}

void DialogueUI::StoreBasicTex()
{
    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mBasicTex);

    SDL_Color black = {0x00, 0x00, 0x00, 0xFF};
    SDL_Color darkgrey = {0x10, 0x10, 0x10, 0xFF};
    
    mPanel->Render(darkgrey, darkgrey);
    mSpeakerBg->Render(mX + 20, mY + 20, nullptr, 160.f, 160.f);
    mSpeakerFrame->Render(mX + 20, mY + 20, nullptr, 160.f, 160.f);

    mDialogueBodyBg->Render(mX + 200, mY + 20, nullptr, 500.f, 160.f);
    mDialogueBodyFrame->Render(mX + 200, mY + 20, nullptr, 500.f, 160.f);

    SDL_SetRenderTarget(System::sRenderer, nullptr);
}

void DialogueUI::RenderOnUpdate()
{
    if (!mIsRender) return;
    //업데이트 상태에 관계없이 기본 텍스처를 렌더링한다.
    SDL_RenderTexture(System::sRenderer, mBasicTex, nullptr, nullptr);
    
    if (!mIsRenderUpdate) {
        SDL_RenderTexture(System::sRenderer, mTempTex, nullptr, nullptr);
        return;
    }
    SDL_Log("dialogue ui render update");

    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);
    //실제 로직
    mSpeakerImg->Render(mX + 20, mY + 20, nullptr, 160.f, 160.f);

    mDialogueBody->Render();

    SDL_SetRenderTarget(System::sRenderer, nullptr);
    mIsRenderUpdate = false;
}

void DialogueUI::SetUI(Texture* pic, TTFWord name, std::string text)
{
    mSpeakerImg = pic; 

    TextUI* tui = mDialogueBody->mTui;
    //이름 설정
    tui->mTexts.push_back(name);
    tui->mTexts.push_back(TTFWord(":", name.mColor, System::sFont));
    tui->mTexts.push_back(TTFWord(System::sFont, TextType::Space));
    //본문
    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
    tui->ProcessAndAddText(text, tc, System::sFont);
}

void DialogueUI::SetUI(Texture *pic)
{
    if (mSpeakerImg == nullptr) return;
    mSpeakerImg->Destroy();
    mSpeakerImg = pic;
}

void DialogueUI::SetUI(std::string text)
{
    TextUI* tui = mDialogueBody->mTui;

    SetUI(mSpkrBlankImg);
    
    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
    tui->ProcessAndAddText(text, tc, System::sFont);
}
