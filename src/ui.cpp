#include "pch.h"
#include "ui.h"

#include "system.h"
#include "render.h"
#include "math.h"
#include "game_context.h"
#include "game_object.h"
#include "game_state.h"
#include "texture.h"
#include "text.h"
#include "shape/point.h"
#include "square.h"
#include "turn.h"
#include "scenario.h"
#include "camera.h"
#include "util.h"
#include "map.h"
#include "entity.h"
#include "skill/skill.h"

void UI::HandleEvent(SDL_Event &e, GameContext &gc, float mouseX, float mouseY)
{
}

void UI::StoreTexture()
{
}

void UI::RenderStoredTex()
{
    if (!mIsRender) return;
    SDL_FRect fr = {(float) mX, (float) mY, (float) mW, (float) mH};
    SDL_RenderTexture(System::sRenderer, mTempTex, nullptr, &fr);
}

void UI::Render()
{
    RenderStoredTex();
    StoreTexture();
}

Button::Button(int x, int y, int w, int h, std::string uiText, BtnType type)
{
    mX = x; mY = y; mW = w; mH = h;
    mUIFrame = new Square(0, 0, w, h);

    mTui = new TextUI(0.f, 0.f);
    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
    mTui->AddWord(TTFWord(uiText, tc, System::sFont));
    
    mType = type;

    TextureManager tm;
    mTempTex = tm.CreateTempTexture(System::sRenderer, w, h);

    mIsRender = true;
}

void Button::HandleEvent(SDL_Event &e, GameContext& gc, float mouseX, float mouseY)
{
    if (!mIsRender) return; //렌더링되고 있지 않으면 반환
    //버튼 안에 있는지 확인
    if (e.button.x < mX) return;
    if (e.button.x > mX + mW) return;    
    if (e.button.y < mY) return;
    if (e.button.y > mY + mH) return;    
    
    //여기에 마우스 오버 이벤트 로직을 입력.

    //클릭했는지 확인
    if (e.type != SDL_EVENT_MOUSE_BUTTON_UP || !System::sIsLeftMouseClicked) return;

    //버튼 타입에 따라 반응함. 헤으응.
    //버튼을 눌렀을 경우
    if (mType == BtnType::OverMap) {
        SDL_Log("overmap button pressed");
        gc.mGsm->mNextState = gc.mGsm->mOms; //다음 타깃 상태는 오버맵 상태다.
        gc.mGsm->mIsStateChange = true;
    }
    else if (mType == BtnType::City) {
        SDL_Log("city button pressed");
        gc.mGsm->mNextState = gc.mGsm->mCvs;
        gc.mGsm->mIsStateChange = true;
    }
    else if (mType == BtnType::Title) {
        SDL_Log("title button pressed");
        gc.mGsm->mNextState = gc.mGsm->mIs; //다음 타깃 상태는 인트로다.
        gc.mGsm->mIsStateChange = true;
    }
    else if (mType == BtnType::SubMap) {
        SDL_Log("change to submap state");
        gc.mGsm->mNextState = gc.mGsm->mSms;
        gc.mGsm->mIsStateChange = true;
    }
    else if (mType == BtnType::NewGame) {
        SDL_Log("new game start");
        gc.mGsm->mNextState = gc.mGsm->mSms;
        gc.mScm->SetCurrentScenario(new NGScenario(), gc);
        gc.mGsm->mIsStateChange = true;
    }
    else if (mType == BtnType::SubMapTurnOver) {
        SDL_Log("submap turn over button pressed");
        gc.mTurnm->UpdateTurn();
    }
    else {
        SDL_Log("button action not specified");
    }
}

void Button::StoreTexture()
{
    if (!mIsRenderUpdate) return;
    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

    mUIFrame->Render();
    mTui->RenderWords();

    SDL_SetRenderTarget(System::sRenderer, nullptr);
    mIsRenderUpdate = false;
}

UIManager::UIManager(GameContext& gc)
{
    mGc = &gc;

    int panelX = System::sWindowWidth * 0.5 - 400;
    int panelY = System::sWindowHeight - 300;

    mToolTip = new ToolTip();
    mDialogueUI = new DialogueUI((float) panelX,(float) panelY);

    mFocusIcon = new IconUI(0, 0, 100, 100, "images/ui/focus.png");
    int x = System::sWindowWidth - 300;
    int y = System::sWindowHeight - 100;
    
    mTurnOverBtn = new Button(x, y, 100, 40, "턴 종료", BtnType::SubMapTurnOver);

    mTileHLUI = new TileHLUI();
    mLogUI = new LogUI(System::sWindowWidth - 280, 100, 240, 800);
    mLogUI->mIsRender = true; //그냥 켜놓음

    mCharacterSheet = new CharacterSheetUI(
        System::sWindowWidth * 0.5 - 600, System::sWindowHeight * 0.5 - 400,
        1200, 800 
    );
    mBCUI = new BottomCharacterUI(
        0, System::sWindowHeight - 120,
        400, 120
    );
    mQSUI = new QuickSkillUI(
        System::sWindowWidth * 0.5 - 400, System::sWindowHeight - 100,
        800, 100, gc
    );
}

void UIManager::InitTopBar()
{
    //탑 바 패널 생성
    mPanels["topPanel"] = new Square(0, 0, System::sWindowWidth, mTopPanelH);

    //ui 객체들 생성
    TextUI* turnTui = new TextUI(70, 0);
    SDL_Color tc = {0x00, 0xD0, 0x00, 0xFF};
}

void UIManager::InitUIs()
{
}

void UIManager::HandleUIEvent(SDL_Event &e, GameContext& gc, float mouseX, float mouseY)
{
    //오른쪽 마우스 버튼 클릭시
    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_RIGHT) {
        //포커스 해제
        gc.mObjm->mEntm->mFocusedEnt = nullptr;
        gc.mObjm->mEntm->mPrevFocusedEnt = nullptr;
        gc.mUim->mFocusIcon->mIsRender = false; //포커스 아이콘 렌더링 안함
        gc.mUim->mCharacterSheet->Deactivate();
        gc.mUim->mQSUI->Deactivate(gc, gc.mMapm->mCurrentMap); //퀵슬롯 비활성화
        return;
    } 

    mCharacterSheet->HandleEvent(e, &gc, mouseX, mouseY);

    for (auto ui : uiMap) {
        ui.second->HandleEvent(e, gc, mouseX, mouseY);
    }

    if (!mDialogueUI->mIsRender) mTurnOverBtn->HandleEvent(e, gc, mouseX, mouseY);
}

void UIManager::HandleMapUIEvent(SDL_Event &e, GameContext& gc, Map *map, float mx, float my)
{
    HandleMapToolTipEvent(e, *gc.mGsm, mx, my);
    mQSUI->HandleEvent(e, gc, map, mx, my);
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

    //기본 ui 렌더링
    for (auto ui : uiMap) {
        ui.second->Render();
    }
}

void UIManager::RenderMapToolTip(Map *map)
{ 
    //카메라 오프셋
    mToolTip->mX = mToolTip->mRefX + mToolTip->mRefW * 0.5 - map->mCam->mSight.x;
    mToolTip->mY = mToolTip->mRefY + mToolTip->mRefH * 0.5 - map->mCam->mSight.y;
    mToolTip->Render();
}

void UIManager::RenderMapUIs(Map* map)
{
    mTileHLUI->RenderBetweenTiles(map);

    mLogUI->Render();

    RenderMapToolTip(map);
    if (!mDialogueUI->mIsRender) mTurnOverBtn->Render();

    mCharacterSheet->Render();
    mQSUI->Render();

    mBCUI->mIsRender = true; //DEBUG
    mBCUI->Render();
}

void UIManager::DestroyUIs()
{
    for (auto ui : uiMap) {
        //ui 텍스처 파괴 로직
        if (ui.second->mTempTex != nullptr) {
            SDL_DestroyTexture(ui.second->mTempTex);
        }
        //ui 자체 메모리 해제
        delete ui.second;
        ui.second = nullptr;
    }

    for (auto square : mPanels) {
        delete square.second;
        square.second = nullptr;
    }

    uiMap.clear();
    mPanels.clear();
}

void UIManager::LoadMapToolTip(Map* map, int tileId)
{
    TextUI* tui = mToolTip->mTui;

    SDL_Color tc {0x00, 0xB0, 0x00, 0xFF};
    tui->AddWord(TTFWord("타일 id:", tc, System::sFont));
    tui->AddWord(TTFWord(System::sFont, TextType::Space));
    tui->AddWord(TTFWord(std::to_string(tileId), tc, System::sFont));
    tui->AddWord(TTFWord(System::sFont, TextType::NewLine));

    //타일 객체를 구함
    MapTile* tile = map->mMapTiles[tileId];
    for (TTFWord* word : tile->mInfos) {
        tui->AddWord(*word);
    }
}

void UIManager::UpdateMapToolTip(Map* map)
{
    mToolTip->CheckUpdate();

    // 툴팁 내부 텍스트, 툴팁이 업데이트 되었을때 로드
    if (mToolTip->mIsRenderUpdate) {

        TextUI* tui = mToolTip->mTui;

        tui->mTexts.clear();
        tui->mTotalWidth = 0; tui->mTotalHeight = 0;

        MapHelper mh;
        int id = mh.WhatTileOnPoint(mToolTip->mRefX, mToolTip->mRefY, map);

        LoadMapToolTip(map, id);
    }
}

void UIManager::UpdateMapToolTip(Map *map, Entity *ent, int targetTileId)
{
    MapTile* ttile = map->mMapTiles[targetTileId];
    ttile->DestroyInfos();
    if (ent->mIsOnMap) mGc->mObjm->mEntm->LoadDataInTile(ttile, ent);
    mToolTip->ClearContent();
    mToolTip->mIsRenderUpdate = true;
}

void UIManager::HandleMapToolTipEvent(SDL_Event &e, GameStateManager &gsm, float mouseX, float mouseY)
{
    //카메라 때문에 생긴 오차 보정
    mouseX += mToolTipMap->mCam->mSight.x;
    mouseY += mToolTipMap->mCam->mSight.y;

    //마우스가 맵 안에 있는지 확인
    Math phs;
    bool mouseIn = phs.IsPointInSquare(mouseX, mouseY, 
        static_cast<float>(mToolTipMap->mX), static_cast<float>(mToolTipMap->mY),
        static_cast<float>(mToolTipMap->mW), static_cast<float>(mToolTipMap->mH)
    );
    //마우스가 맵 안에 있으면 렌더링함.
    if (mouseIn) {
        mToolTip->mIsRender = true;
        
        if (!mWasMouseOnMap) {
            SDL_Log("mouse in map");
            mToolTip->mIsRenderUpdate = true;
            mWasMouseOnMap = true;
        }
    }
    else {
        mToolTip->mIsRender = false;
        mWasMouseOnMap = false;
    } 

    //mouseover 중인 타일의 id를 구함
    MapHelper mh;
    int id = mh.WhatTileOnPoint(mouseX, mouseY, mToolTipMap);

    MapTile* tile = mToolTipMap->mMapTiles[id];
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
    if (mUIFrame) delete mUIFrame;
}

void ToolTip::ClearContent()
{
    mTui->mTexts.clear();
}

void ToolTip::SetToolTipFrame()
{
    float currentW = 0.f, currentH = 0.f;
    float maxW = 0.f, maxH = 0.f;
    bool firstWord = true; //첫 단어에서 사용되고 거짓으로 바뀜

    for (auto pair : mTui->mTexts) {
        //폰트 높이 캐싱
        int fontH = TTF_GetFontHeight(pair.second.mFont);
        //단어가 있으면 높이 추가
        if (firstWord) {
            maxH += fontH;
            firstWord = false;
        }

        //띄어쓰기일때
        if (pair.second.mType == TextType::Space) {
            currentW += fontH * 0.5; 
        }
        //줄바꿈일때
        if (pair.second.mType == TextType::NewLine) {

            if (maxW <= currentW) maxW = currentW; //최대값 캐싱
            currentW = 0; //초기화
            maxH += fontH;
            maxH += mTui->mLineSpacing;
        }
        //문자일때
        else {
            currentW += pair.second.GetWordWidth();
        }
    }
    if (maxW <= currentW) maxW = currentW; //최대값 캐싱
    maxW += mPadding * 2; //패딩 추가
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
    if (mIsRenderUpdate) {
        return;
    }

    //저장된 좌표 정보가 현재 참조 좌표 정보와 일치함
    if (mPrevX == mRefX && mPrevY == mRefY) {

    }
    //새로운 좌표로 이동했음
    else {
        mIsRenderUpdate = true;
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
}

void ToolTip::StoreTexture()
{
    if (!mIsRenderUpdate) return;
    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

    //실제 텍스처 렌더링 동작
    mUIFrame->SetX(0.f); mUIFrame->SetY(0.f); //위치 설정
    SetToolTipFrame(); //동적 크기 설정
    mTui->mX = 0.f; mTui->mY = 0.f;

    mUIFrame->Render(0x00, 0xB0, 0x00, 0xFF, 0x08, 0x08, 0x08, 0xD0);
    mTui->RenderWords();

    //타겟 해제 지점
    SDL_SetRenderTarget(System::sRenderer, nullptr);
    mIsRenderUpdate = false;
}

void ToolTip::RenderStoredTex()
{
    if (!mIsRender) return;
    SDL_FRect fr = {mX, mY, static_cast<float>(System::sWindowWidth), static_cast<float>(System::sWindowHeight)};
    SDL_RenderTexture(System::sRenderer ,mTempTex, nullptr, &fr);
}

void ToolTip::Render()
{
    RenderStoredTex();
    StoreTexture();
}

TextUI::TextUI(float x, float y)
{
    mX = x; mY = y;
}

void TextUI::ClearTexts()
{
    mTexts.clear();
    mTotalWidth = 0;
    mTotalHeight = 0;
}

void TextUI::AddWord(TTFWord word)
{
    mTexts.emplace(mWordId, word);
    mWordId++;
}

void TextUI::ProcessAndAddText(std::string text, SDL_Color color, TTF_Font *font)
{
    std::string message = "";

    for (int i = 0; i < text.length(); i++) {
        //공백일 경우
        if (text.substr(i, 1) == " "){
            //텍스트를 푸시하고 공백도 푸시함
            AddWord(TTFWord(message, color, font));
            AddWord(TTFWord(font, TextType::Space));

            message = ""; //초기화
        }
        //한글 등 문자당 3바이트를 쓰는 언어일 경우
        else if ((text[i] & 0b11110000) == 0b11100000) {
            message += text.substr(i, 3);

            //문장 끝에 도달함
            if (i == text.length() - 3) {
                AddWord(TTFWord(message, color, font));
                return;
            }

            i += 2;
        }
        //문자당 1바이트인 언어일 경우
        else {
            message += text.substr(i, 1);

            //문장 끝에 도달함
            if (i == text.length() - 1) {
                AddWord(TTFWord(message, color, font));
                return;
            }
        }
    }
}

void TextUI::RenderWords()
{
    mTotalWidth = 0; mTotalHeight = 0;
    for (auto pair : mTexts) {
        if (pair.second.mType == TextType::NewLine) NewLine(pair.second.mFont);
        else if (pair.second.mType == TextType::Space) AddSpace(pair.second.mFont);
        else RenderAtLine(pair.second);
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

FramedTUI::FramedTUI(int x, int y, int w, int h)
{
    mX = x; mY = y; mW = w; mH = h;

    mTui = new TextUI(x, y);
}

void FramedTUI::AddWordAndProcess(TTFWord word)
{
    mTui->ProcessAndAddText(word.mMessage, word.mColor, word.mFont);
}

void FramedTUI::AddWord(TTFWord word)
{
    mTui->AddWord(word);
}

void FramedTUI::ClearText()
{
    mTui->mTexts.clear();
}

void FramedTUI::Render()
{
    //프레임에 맞춰서 줄바꿈

    mTui->mTotalHeight = 0;
    mTui->mTotalWidth = 0;
    mTotalW = 0;
    mTotalH = 0;
    for (auto pair : mTui->mTexts) {
        TTFWord word = pair.second;
        if (pair.second.mType == TextType::NewLine) {
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
void FramedTUI::HandleEvent(SDL_Event &e, GameContext& gc, float mouseX, float mouseY)
{
}

IconUI::IconUI(int x, int y, int width, int height, std::string path)
{
    mX = x; mY = y; mW = width; mH = height;
    mTex = new Texture(path);
    
    SDL_SetTextureBlendMode(mTex->mTexture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
}

void IconUI::Render()
{
    if (!mIsRender) return;

    mTex->Render(mX, mY, nullptr, mW, mH);
}

void IconUI::RenderByCam(Camera *cam)
{
    if (!mIsRender) return;

    mTex->Render(mX - cam->mSight.x, mY - cam->mSight.y, nullptr, mW, mH);
}

void IconUI::HandleEvent(SDL_Event &e, GameContext& gc, float mouseX, float mouseY)
{
    //버튼 안에 있는지 확인
    if (e.button.x < mX) return;
    if (e.button.x > mX + mW) return;    
    if (e.button.y < mY) return;
    if (e.button.y > mY + mH) return;    
    
    //여기에 마우스 오버 이벤트 로직을 입력.

    //클릭했는지 확인
    if (e.type != SDL_EVENT_MOUSE_BUTTON_UP || !System::sIsLeftMouseClicked) return;
}

void IconUI::SetDimension(int x, int y, int w, int h)
{
    mX = x; mY = y; mW = w; mH = h;
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
    mDialogueBody->mIsRender = true;

    mDialogueBodyFrame = new Texture("images/ui/dialogue_frame.png");

    StoreBasicTex();
}

void DialogueUI::HandleEvent(SDL_Event &e, GameContext& gc, float mouseX, float mouseY)
{
    if (!mIsRender) return; //렌더링되지 않으면 이벤트 핸들링도 하지 않음

    Math ps;
    bool mouseIn = ps.IsPointInSquare(mouseX, mouseY,
         mPanel->GetX(), mPanel->GetY(), (float) mPanel->GetW(), (float) mPanel->GetH()
    );
    
    if (!mouseIn) return;
    //마우스 오버

    if (e.type != SDL_EVENT_MOUSE_BUTTON_UP || !System::sIsLeftMouseClicked) return;
    //클릭 동작

    SDL_Log("clicked dialogue panel");
    gc.mScm->mCurrentSc->mIsDialogueUpdate = true;
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
    tui->AddWord(name);
    tui->AddWord(TTFWord(":", name.mColor, System::sFont));
    tui->AddWord(TTFWord(System::sFont, TextType::Space));
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

TileHLUI::TileHLUI()
{
    mHighlight = new Texture("images/ui/highlight.png");
    SDL_SetTextureBlendMode(mHighlight->mTexture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
    TextureManager tm;
}

TileHLUI::~TileHLUI()
{
    mHighlight->Destroy();
}

void TileHLUI::SetTileIds(std::vector<int> ids)
{
    mTIds = ids;
}

void TileHLUI::ClearTileIds()
{
    mTIds.clear();
}

void TileHLUI::Update()
{
}

void TileHLUI::RenderBetweenTiles(Map* map)
{
    if (!mIsRenderBetweenTiles) return;

    MapHelper mh;
    
    for (int id : mTIds) {
        MapTile* tile = map->mMapTiles[id];
        //카메라 오프셋
        mHighlight->Render(
            (float) tile->mX - map->mCam->mSight.x,
            (float) tile->mY - map->mCam->mSight.y, nullptr,
            (float) map->mTileLen, (float) map->mTileLen
        );
    }
}

CharacterSheetUI::CharacterSheetUI(int x, int y, int w, int h)
{
    mX = x; mY = y; mW = w; mH = h;

    mTex = new Texture("images/ui/character_sheet.png");
    SDL_SetTextureScaleMode(mTex->mTexture, SDL_SCALEMODE_NEAREST);

    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
    for (int i = 0; i < 16; i ++) {
        FramedTUI* skill = new FramedTUI(0, 40 + i * 40, mW * 0.5, 40);
        skill->AddWord(TTFWord("-", tc, System::sFont));
        mSkillList.push_back(skill);
    }

    mSkillDesc = new FramedTUI(mW * 0.5, 40, mW * 0.5, mH - 40);
    mSkillDesc->AddWord(TTFWord("스킬 설명란이다.", tc, System::sFont));
 
    TextureManager tm;
    mTempTex = tm.CreateTempTexture(System::sRenderer, w, h);
}

void CharacterSheetUI::Activate(Entity* ent)
{
    mCanHandleEvent = true;
    mIsRender = true;
    UpdateUI(ent);
}

void CharacterSheetUI::Deactivate()
{
    mCanHandleEvent = false;
    mIsRender = false;
}

void CharacterSheetUI::HandleEvent(SDL_Event &e, GameContext *gc, float mx, float my)
{
    if (!mCanHandleEvent) return;

    Math mth;
    bool isIn = mth.IsPointInSquare(mx, my, (float) mX, (float) mY, (float) mW, (float) mH);
    if (!isIn) return;
    //마우스오버
    HandleSkillListEvent(e, gc, mx, my);

    gc->mMapm->mCurrentMap->mCanHandleEvent = false;
    gc->mUim->mToolTip->mIsRender = false;
}

void CharacterSheetUI::HandleSkillListEvent(SDL_Event &e, GameContext *gc, float mx, float my)
{
    Math mth;
    Entity* focused = gc->mObjm->mEntm->mFocusedEnt;
    if (!focused) return;

    for (int i = 0; i < (int) mSkillList.size(); i++) {
        FramedTUI* s = mSkillList[i];
        bool isMouseIn = mth.IsPointInSquare(mx, my, (float) (mX + s->mX), (float) (mY + s->mY), (float) s->mW, (float) s->mH);
        if (!isMouseIn) continue;
        //마우스오버
        UpdateSkillDesc(i, gc);

        if (e.type != SDL_EVENT_MOUSE_BUTTON_UP || !System::sIsLeftMouseClicked) return;
        //클릭
        //TODO:퀵슬롯에 스킬을 넣는다.
        std::string message = "character sheet: skill list index is " + std::to_string(i);
        SDL_Log(message.c_str());
        if (i >= (int) focused->mSkills.size()) continue;
        Skill* skill = focused->mSkills[i];
        gc->mUim->mQSUI->AddSkill(skill);
    }
}

void CharacterSheetUI::UpdateUI(Entity *ent)
{
    int i = 0;
    for (Skill* skill : ent->mSkills) {
        mSkillList[i]->mTui->mTexts[0].mMessage = skill->mName;
        i++;
    }

    mIsRenderUpdate = true;
}

void CharacterSheetUI::UpdateSkillDesc(int idx, GameContext* gc)
{
    Entity* focused = gc->mObjm->mEntm->mFocusedEnt;
    if (!focused) return;
    if (idx >= (int) focused->mSkills.size()) return;

    Skill* tskill = focused->mSkills[idx];

    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};

    mSkillDesc->ClearText();
    mSkillDesc->AddWord(TTFWord(tskill->mName, tc, System::sFont));
    mIsRenderUpdate = true;
}

void CharacterSheetUI::StoreTexture()
{
    if (mIsRenderUpdate == false) return;
    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

    Texture t;
    mTex->Render(0.f, 0.f, nullptr,(float) mW,(float) mH);

    for (FramedTUI* skill : mSkillList) {
        skill->Render();
    }
    mSkillDesc->Render();

    SDL_SetRenderTarget(System::sRenderer, nullptr);
    mIsRenderUpdate = false;
}

CharacterSkillUI::CharacterSkillUI()
{
}

QuickSkillUI::QuickSkillUI(int x, int  y, int w, int h, GameContext& gc)
{
    mX = x; mY = y; mW = w; mH = h;

    TextureManager tm;
    mTempTex = tm.CreateTempTexture(System::sRenderer, mW, mH);

    mGc = &gc;
}

void QuickSkillUI::AddSkill(Skill* skill)
{
    Entity* focused = mGc->mObjm->mEntm->mFocusedEnt;
    if (!focused) return;
    if (!focused->mIsPawn) return;

    Pawn* p = static_cast<Pawn*> (focused);

    if (p->mQuickSkills.size() > System::sQuickSlotCap) {
        SDL_Log("quick skill ui: cannot add skill, size limit hit");
        return;
    } 
    for (auto iter = p->mQuickSkills.begin(); iter != p->mQuickSkills.end();) {
        //삼입하려는 스킬 코드가 이미 퀵슬롯 ui에 있을 경우
        if (*iter == skill->mCode) {
            //해당 친구를 컨테이너에서 삭제하고 리턴한다.
            p->mQuickSkills.erase(iter);
            mIsRenderUpdate = true;
            SDL_Log("quick skill ui: erased skill code");
            return;
        }
        iter++;
    }

    p->mQuickSkills.push_back(skill->mCode);
    mIsRenderUpdate = true;
    SDL_Log("quick skill ui: added skill code");
}

void QuickSkillUI::StoreTexture()
{
    if (!mIsRenderUpdate) return;

    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

    Texture t;
    t.LoadFromFile("images/black.png");
    t.Render(0.f, 0.f, nullptr, (float) mW, (float) mH);

    t.LoadFromFile("images/frame.png"); 
    for (int i = 0; i < 8; i++) {
        t.Render((float) (mPadding + i * 80), (float) (mPadding), nullptr, 80.f, 80.f);
    }

    //포커스된 pc가 있는 경우
    Entity* focused = mGc->mObjm->mEntm->mFocusedEnt;
    if (!focused) {}
    else if (focused->mIsPawn) {
        Pawn* p = static_cast<Pawn*> (focused);
        json skillDb = mGc->mSkm->mSkillDb["items"];
        int i = 0;
        for (std::string code : p->mQuickSkills) {
            if (i >= System::sQuickSlotCap) break;
            if (skillDb.contains(code)) {
                std::string path = skillDb[code]["img_path"].get<std::string>();
                t.LoadFromFile(path);
                //스킬 아이콘을 렌더링
                t.Render((float) (mPadding + i * 80), (float) (mPadding), nullptr, 80.f, 80.f);
            }
            else {
                std::string message = "quick skill ui: cannot find skill code \"" + code + "\" in skill db";
                SDL_Log(message.c_str());
            }
            i++;
        }
    }

    SDL_SetRenderTarget(System::sRenderer, nullptr);
    mIsRenderUpdate = false;
}

void QuickSkillUI::HandleEvent(SDL_Event &e, GameContext& gc, Map* map, float mouseX, float mouseY)
{
    if (!mCanHandleEvent) return;

    Entity* focused = gc.mObjm->mEntm->mFocusedEnt;
    if (!focused) return;
    if (!focused->mIsPawn) return;

    Pawn* p = static_cast<Pawn*> (focused);

    Math mth;
    bool mouseIn = mth.IsPointInSquare(mouseX, mouseY, (float) mX, (float) mY, (float) mW, (float) mH);
    //마우스가 ui안에 있으면 맵 이벤트핸들링 안함.
    if (!mouseIn) {
        map->mCanHandleEvent = true;
        return;
    }
    else {
        map->mCanHandleEvent = false;
        gc.mUim->mTileHLUI->mIsRenderBetweenTiles = false;
    }

    if (!System::sIsLeftMouseClicked || e.type != SDL_EVENT_MOUSE_BUTTON_UP) return;

    float xDis = mouseX - (float) mX;
    int xPos = xDis/80;

    if (xPos < 0) xPos = 0;
    if (xPos > System::sQuickSlotCap - 1) xPos = System::sQuickSlotCap - 1;

    if (p->mQuickSkills.size() > xPos) {
        //스킬 발동을 위한 준비 단계
        //인덱스에 따라서 스킬 코드를 가져온다.
        std::string skillCode = p->mQuickSkills[xPos];
        //TODO: 여기부터는 따로 함수로 래핑하는게 좋을듯
        //엔티티에 저장된 실제 스킬 객체를 찾아온다.
        Skill* skill = gc.mObjm->mEntm->mFocusedEnt->mSkills[xPos];

        //현재 스킬에 따라서 맵 타일 하이라이트 색을 바꿔준다.
        json skillTable = gc.mSkm->mSkillDb["items"];
        if (!skillTable.contains(skillCode)) {
            SDL_Log("quick skill ui: cannot find skill code in skill database!");
        }
        else {
            json sd = skillTable[skillCode];
            std::string stype = sd["type"].get<std::string>();
            if (stype == "movement") gc.mUim->mTileHLUI->mHighlight->LoadFromFile("images/ui/highlight.png");
            else if (stype == "attack") gc.mUim->mTileHLUI->mHighlight->LoadFromFile("images/ui/highlight_red.png");
            else SDL_Log("quick skill ui: unknown skill type!");

            mGc->mSkm->SetSkillData(sd); //스킬 데이터 캐싱
            mGc->mSkm->mIsSkillReady = true; //스킬 사용 준비 완료
        }

        mGc->mSkm->SetSkill(skill); //스킬 객체 캐싱
        mGc->mSkm->SetActor(gc.mObjm->mEntm->mFocusedEnt); //액터 캐싱

        map->mCanHandleEvent = true;
    }
}

void QuickSkillUI::Activate(GameContext& gc, Map *map, Pawn *pawn)
{
    mCanHandleEvent = true;
    mIsRenderUpdate = true;
    mIsRender = true;
}

void QuickSkillUI::Deactivate(GameContext& gc, Map* map)
{
    mCanHandleEvent = false;
    mIsRender = false; //스킬 퀵슬롯 렌더링 안함
    map->mCanHandleEvent = true;
}

BottomCharacterUI::BottomCharacterUI(int x, int y, int w, int h)
{
    mX = x; mY = y; mW = w; mH = h;

    mMainStat = new FramedTUI(0, 0, w * 0.5, h);
    mStatEffect = new FramedTUI(w * 0.5, 0, w * 0.5, h);
    SDL_Color c = {0x00, 0xB0, 0x00, 0xFF};
    SDL_Color r = {0xB0, 0x40, 0x40, 0xFF};
    SDL_Color g = {0x40, 0xB0, 0x40, 0xFF};
    SDL_Color b = {0x40, 0x40, 0xB0, 0xFF};
    SDL_Color wg = {0xB0, 0xB0, 0xB0, 0xFF};

    mMainStat->AddWord(TTFWord("이름", c, System::sFont));
    mMainStat->AddWord(TTFWord(System::sFont, TextType::NewLine));

    mMainStat->AddWord(TTFWord("HP:", r, System::sFont));
    mMainStat->AddWord(TTFWord(System::sFont, TextType::Space));
    mMainStat->AddWord(TTFWord("0", wg, System::sFont));
    mMainStat->AddWord(TTFWord("/0", wg, System::sFont));
    mMainStat->AddWord(TTFWord(System::sFont, TextType::NewLine));

    mMainStat->AddWord(TTFWord("SP:", g, System::sFont));
    mMainStat->AddWord(TTFWord(System::sFont, TextType::Space));
    mMainStat->AddWord(TTFWord("0", wg, System::sFont));
    mMainStat->AddWord(TTFWord("/0", wg, System::sFont));
    mMainStat->AddWord(TTFWord(System::sFont, TextType::NewLine));

    mMainStat->AddWord(TTFWord("AP:", b, System::sFont));
    mMainStat->AddWord(TTFWord(System::sFont, TextType::Space));
    mMainStat->AddWord(TTFWord("0", wg, System::sFont));
    mMainStat->AddWord(TTFWord("/0", wg, System::sFont));

    mStatEffect->AddWord(TTFWord("건강한 아이입니다.", wg, System::sFont));

    TextureManager tm;
    mTempTex = tm.CreateTempTexture(System::sRenderer, mW, mH);
}

void BottomCharacterUI::UpdateUI(Entity *ent)
{
    if (!ent) return;
    StatHelper sh;

    std::map<int, TTFWord> &words = mMainStat->mTui->mTexts;
    words[static_cast<int>(BCUIMainStatIdx::Name)].mMessage = ent->mName;
    words[static_cast<int>(BCUIMainStatIdx::CurHp)].mMessage = std::to_string(ent->mCurHp);
    words[static_cast<int>(BCUIMainStatIdx::MaxHp)].mMessage = "/" + std::to_string(sh.GetMaxHp(ent));
    words[static_cast<int>(BCUIMainStatIdx::CurSp)].mMessage = std::to_string(ent->mCurSp);
    words[static_cast<int>(BCUIMainStatIdx::MaxSp)].mMessage = "/" + std::to_string(sh.GetMaxSp(ent));
    words[static_cast<int>(BCUIMainStatIdx::CurAp)].mMessage = std::to_string(ent->mCurAp);
    words[static_cast<int>(BCUIMainStatIdx::MaxAp)].mMessage = "/" + std::to_string(sh.GetMaxAp(ent));

    mIsRenderUpdate = true;
}

void BottomCharacterUI::StoreTexture()
{
    if (!mIsRenderUpdate) return;
    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

    Texture t;
    t.LoadFromFile("images/ui/bottom_char_ui.png");
    t.Render(0.f, 0.f, nullptr, (float) mW, (float) mH);
    mMainStat->Render();
    mStatEffect->Render();

    SDL_SetRenderTarget(System::sRenderer, nullptr);
    mIsRenderUpdate = false;
}

LogUI::LogUI(int x, int y, int w, int h)
{
    mX = x; mY = y; mW = w; mH = h;

    mBody = new FramedTUI(0, 0, w, h);
    mBody->mTui->mLineSpacing = 6;

    TextureManager tm; 
    mTempTex = tm.CreateTempTexture(System::sRenderer, mW, mH);
}

void LogUI::AddMessage(std::string message, SDL_Color c)
{
    mBody->AddWordAndProcess(TTFWord(message, c, System::sFont));
    mBody->AddWord(TTFWord(System::sFont, TextType::NewLine));
    mIsRenderUpdate = true;
}

void LogUI::StoreTexture()
{
    if (!mIsRenderUpdate) return;
    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

    Texture* bg = new Texture("images/ui/log.png");
    bg->Render(0.f, 0.f, nullptr, (float) mW, (float) mH);
    mBody->Render();

    SDL_SetRenderTarget(System::sRenderer, nullptr);
    mIsRenderUpdate = false;
}
