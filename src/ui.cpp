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
#include "item/item.h"
#include "item/item_manager.h"
#include "skill/skill.h"
#include "event_context.h"

void UI::HandleEvent(SDL_Event &e, GameContext &gc, float mouseX, float mouseY)
{
}

void UI::Activate()
{
}
void UI::Deactivate()
{
}

void UI::RenderThings()
{
}

void UI::StoreTexture()
{
    if (!mIsRenderUpdate) return;
    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

    RenderThings();

    SDL_SetRenderTarget(System::sRenderer, nullptr);
    mIsRenderUpdate = false;
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

    mTui = new TextUI((float) x, (float) y);
    TTFWord word = TTFWord(uiText, System::kTc, System::sFont);
    mTui->AddWord(word);
    //가운데 위치에 정렬하는 작업
    //가로 정렬
    int len = word.GetWordWidth();
    int diff = w - len;
    if (diff < 0) diff = 0;
    mTui->mX = diff * 0.5 - mPadding;
    if (mTui->mX < 0) mTui->mX = 0;
    //세로 정렬
    diff = h - TTF_GetFontHeight(word.mFont);
    if (diff < 0) diff = 0;
    mTui->mY = diff * 0.5 - mPadding;
    if (mTui->mY < 0) mTui->mY = 0;

    mType = type;

    TextureManager tm;
    mTempTex = tm.CreateTempTexture(System::sRenderer, w, h);

    mIsRender = true;
    mIsRenderUpdate = true;
}

Button::~Button()
{
    Destroy();
}

void Button::Destroy()
{
    if (mTempTex) SDL_DestroyTexture(mTempTex);
    if (mTui) delete mTui;
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

bool Button::IsMouseIn(float mx, float my)
{
    Math mth;
    bool isIn = mth.IsPointInSquare(mx, my,(float) mX,(float) mY,(float) mW,(float) mH);

    return isIn;
}

void Button::RenderThings()
{
    mTui->RenderWords();
}

void Button::RenderThings(int x, int y)
{
    mTui->RenderWords(x, y);
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
    
    mTurnOverBtn = new Button(x, y, 180, 40, "턴 종료(SPACE)", BtnType::SubMapTurnOver);

    mTileHLUI = new TileHLUI();
    mLogUI = new LogUI(System::sWindowWidth - 280, 100, 240, 800);
    mLogUI->mIsRender = true; //그냥 켜놓음

    mCharacterSheet = new CharacterSheetUI(
        System::sWindowWidth * 0.5 - 600, System::sWindowHeight * 0.5 - 400,
        1200, 800, &gc 
    );
    mItemMenu = new ItemMenu(
        System::sWindowWidth * 0.5 - 50, System::sWindowHeight * 0.5 - 100,
        100, 200, &gc
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

    //ui 객체들 생성
    TextUI* turnTui = new TextUI(70, 0);
    SDL_Color tc = {0x00, 0xD0, 0x00, 0xFF};
}

void UIManager::InitUIs()
{
}

void UIManager::HandleUIEvent(SDL_Event &e, GameContext& gc, float mouseX, float mouseY)
{
    //esc 눌렀을때
    if (e.key.key == SDLK_ESCAPE && e.type == SDL_EVENT_KEY_DOWN) {
        PopBackUI();
        return;
    } 

    bool isConsumed {false};

    isConsumed = mItemMenu->HandleEvent(e, mouseX, mouseY);
    if (isConsumed) return;
    mCharacterSheet->HandleEvent(e, gc, mouseX, mouseY);

    for (auto ui : uiMap) {
        ui.second->HandleEvent(e, gc, mouseX, mouseY);
    }

    if (!mDialogueUI->mIsRender) mTurnOverBtn->HandleEvent(e, gc, mouseX, mouseY);
}

void UIManager::HandleMapUIEvent(SDL_Event &e, GameContext& gc, Map *map, float mx, float my)
{
    HandleMapToolTipEvent(e, *gc.mGsm, mx, my);
    mQSUI->HandleEvent(e, gc, map, mx, my);
    gc.mTurnm->HandleEvent(e, mx, my);
}

void UIManager::RenderUIs()
{
    SDL_SetRenderLogicalPresentation(System::sRenderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);

    SDL_Color fillColor = {0x08, 0x08, 0x08, 0xF0};
    SDL_Color lineColor = {0x00, 0x80, 0x00, 0xFF};

    //ui 패널 렌더링

    //기본 ui 렌더링
    for (auto ui : uiMap) {
        ui.second->Render();
    }
}

void UIManager::RenderMapToolTip(Grid *grid)
{ 
    //카메라 오프셋
    mToolTip->mX = mToolTip->mRefX + mToolTip->mRefW * 0.5 - grid->mCam->mSight.x;
    mToolTip->mY = mToolTip->mRefY + mToolTip->mRefH * 0.5 - grid->mCam->mSight.y;
    mToolTip->Render();
}

void UIManager::RenderMapUIs(Map* map)
{
    mTileHLUI->RenderBetweenTiles(map);

    mLogUI->Render();

    if (!mDialogueUI->mIsRender) mTurnOverBtn->Render();
    
    mCharacterSheet->Render();
    mQSUI->Render();
    RenderMapToolTip(mToolTipGrid);
    mItemMenu->Render();

    mBCUI->mIsRender = true; //DEBUG
    mBCUI->Render();
}

void UIManager::PopBackUI()
{
    if (!mUIStack.empty()) {            
        mUIStack.back()->Deactivate();
        mUIStack.pop_back();
        
        if (!mUIStack.empty()) mUIStack.back()->mCanHandleEvent = true;
        return;
    } 
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

    uiMap.clear();
}

void UIManager::LoadInvToolTip(Grid *grid, int tileId)
{
    UIHelper uh;
    TextUI* tui = mToolTip->mTui;
    Entity* focused = mGc->mObjm->mEntm->mFocusedEnt;

    SDL_Color tc {0x00, 0xB0, 0x00, 0xFF};
    tui->AddWord(TTFWord("타일 id:", tc, System::sFont));
    tui->AddWord(TTFWord(System::sFont, TextType::Space));
    tui->AddWord(TTFWord(std::to_string(tileId), tc, System::sFont));
    tui->AddWord(TTFWord(System::sFont, TextType::NewLine));    

    Pawn* focusedP = static_cast<Pawn*>(focused);
    for (auto itemPair : focusedP->mInventory) {
        Item* item = itemPair.second;
        if (item->mTileId == tileId) {
            tui->AddWord(TTFWord(item->mName, System::kWh, System::sFont));
            tui->AddWord(TTFWord(System::sFont, TextType::NewLine));    
            tui->AddWord(TTFWord("가치: " + std::to_string(item->mValue), System::kWh, System::sFont));
            tui->AddWord(TTFWord(System::sFont, TextType::NewLine));        
        }
    }
}

void UIManager::LoadMapToolTip(Map *map, int tileId)
{
    UIHelper uh;
    TextUI* tui = mToolTip->mTui;
    MapTile* tile = map->mMapTiles[tileId];

    SDL_Color tc {0x00, 0xB0, 0x00, 0xFF};
    tui->AddWord(TTFWord("타일 id:", tc, System::sFont));
    tui->AddWord(TTFWord(System::sFont, TextType::Space));
    tui->AddWord(TTFWord(std::to_string(tileId), tc, System::sFont));
    tui->AddWord(TTFWord(System::sFont, TextType::NewLine));

    for (Entity* ent : map->mNpcs) {
        if (ent->mTileId == tileId) {
            uh.AddEntityData(tui, ent);
        }
    }
    for (Entity* p : map->mPawns) {
        if (p->mTileId == tileId) {
            uh.AddEntityData(tui, p);
        }   
    }

    //아이템 스택이 맵에 존재하는 경우
    if (map->mItemStackMap.find(tileId) != map->mItemStackMap.end()) {
        ItemStack* stackObj = map->mItemStackMap[tileId];
        //스택이 빈 경우 아무것도 하지 않음
        if (stackObj->mStack.empty()) {}
        //스택에 아이템이 하나인 경우 아이템 이름을 추가함
        else if ((int) stackObj->mStack.size() == 1) {
            tui->AddWord(TTFWord (stackObj->mStack[0]->mName, System::kWh, System::sFont));
            tui->AddWord(TTFWord(System::sFont, TextType::NewLine));
        }
        //스택에 아이템이 하나 이상인 경우 첫번째 아이템 이름을 렌더링하고 생략된 표현을 추가함
        else {
            tui->AddWord(TTFWord (stackObj->mStack.back()->mName + "...", System::kWh, System::sFont));
            tui->AddWord(TTFWord(System::sFont, TextType::NewLine));
        }
    }
}

void UIManager::UpdateGridToolTip(Grid* grid)
{
    mToolTip->CheckUpdate();

    // 툴팁 내부 텍스트, 툴팁이 업데이트 되었을때 로드
    if (mToolTip->mIsRenderUpdate) {

        TextUI* tui = mToolTip->mTui;

        tui->mTexts.clear();
        tui->mTotalWidth = 0; tui->mTotalHeight = 0;

        MapHelper mh;
        int id = mh.WhatTileOnPoint(mToolTip->mRefX, mToolTip->mRefY, grid);

        if (grid->mGridType == GridType::Map) {
            Map* map = static_cast<Map*>(grid);
            LoadMapToolTip(map, id);
        }

        else if (grid->mGridType == GridType::Inventory) {
            LoadInvToolTip(grid, id);
        }
    }
}

void UIManager::HandleMapToolTipEvent(SDL_Event &e, GameStateManager &gsm, float mouseX, float mouseY)
{
    if (!mCanHandleToolTip) return;

    //카메라 때문에 생긴 오차 보정
    mouseX += mToolTipGrid->mCam->mSight.x;
    mouseY += mToolTipGrid->mCam->mSight.y;

    //마우스가 맵 안에 있는지 확인
    Math phs;
    bool mouseIn = phs.IsPointInSquare(mouseX, mouseY, 
        static_cast<float>(mToolTipGrid->mOffsetX + mToolTipGrid->mX),
        static_cast<float>(mToolTipGrid->mOffsetY + mToolTipGrid->mY),
        static_cast<float>(mToolTipGrid->mW), static_cast<float>(mToolTipGrid->mH)
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
    int id = mh.WhatTileOnPoint(mouseX, mouseY, mToolTipGrid);

    int tl = mToolTipGrid->mTileLen;
    std::unordered_map<std::string, int> xy = mh.PosXYByTileId(id, mToolTipGrid);
    //타일 좌표를 툴팁의 참조 좌표에 할당해줌
    mToolTip->SetRefInfo(
        mToolTipGrid->mOffsetX + mToolTipGrid->mX + xy["x"]*tl,
        mToolTipGrid->mOffsetY + mToolTipGrid->mY + xy["y"]*tl,
        tl, tl
    );
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
        mX = mRefX + mRefW * 0.5; //가운데쯤에 생성
        mY = mRefY + mRefH * 0.5;
        
        mPrevX = mRefX; mPrevY = mRefY; //좌표 정보 다시 캐싱
        mPrevW = mRefW; mPrevH = mRefH; 
    }
}

void ToolTip::RenderThings(float x, float y)
{
    mUIFrame->SetX(x); mUIFrame->SetY(y); //위치 설정
    SetToolTipFrame(); //동적 크기 설정
    mTui->mX = x; mTui->mY = y;

    mUIFrame->Render(0x00, 0xB0, 0x00, 0xFF, 0x08, 0x08, 0x08, 0xD0);
    mTui->RenderWords();
}

void ToolTip::StoreTexture()
{
    if (!mIsRenderUpdate) return;
    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

    //실제 텍스처 렌더링 동작
    RenderThings(0.f, 0.f);

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

TextUI::TextUI(float x, float y)
{
    mX = x; mY = y;
}

int TextUI::GetHeight()
{
    int ret {0};
    for (auto textPair : mTexts) {
        if (textPair.second.mType == TextType::NewLine) {
            ret += TTF_GetFontHeight(textPair.second.mFont);
            ret += mLineSpacing;
        }
    }

    return ret;
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
    RenderWords(0, 0);
}

void TextUI::RenderWords(int x, int y)
{
    mTotalWidth = x; mTotalHeight = y;
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
        //디폴트 타입일 경우
        else {
            mTotalW += word.GetWordWidth();
            //넓이 검사
            //텍스트 길이가 너비를 넘어감
            if (mTotalW + mPadding * 2 > mW) {
                //너비를 초과할 때까지 렌더링 후 줄바꿈
                int diff = (mTotalW + mPadding * 2) - mW; //초과된 길이만큼 구함
                int remain = word.GetWordWidth() - diff; //이번 줄에 남은 길이를 구함
                while (true) {
                    int firstW = word.GetFirstWordWidth(); //맨 앞 글자의 길이를 가져옴
                    if (remain - firstW < 0) break; //remain이 음수가 되기전에 탈출함
                    if (firstW == TTF_GetFontHeight(word.mFont)) {
                        //3바이트 문자일 경우
                        mTui->RenderAtLine(TTFWord(word.mMessage.substr(0, 3), word.mColor, word.mFont));
                        word.mMessage = word.mMessage.erase(0, 3);
                    }
                    else if (firstW == TTF_GetFontHeight(word.mFont) * 0.5) {
                        //1바이트 문자
                        mTui->RenderAtLine(TTFWord(word.mMessage.substr(0, 1), word.mColor, word.mFont));
                        word.mMessage = word.mMessage.erase(0, 1);
                    }

                    remain -= firstW;
                }

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

InventoryUI::InventoryUI(int x, int y, int offsetX, int offsetY, int w, int h, int tileLen, GameContext* gc)
{
    mGc = gc;
    mToolTip = new ToolTip();

    mX = x; mY = y; mW = w; mH = h;
    mOffsetX = offsetX; mOffsetY = offsetY;

    int remain = w % tileLen;
    
    mGrid = new Grid(x, y, remain * 0.5 + 6 * tileLen + offsetX, offsetY, 12, 10, tileLen, GridType::Inventory);

    TextureManager tm;
    mTempTex = tm.CreateTempTexture(System::sRenderer, w, h);

    int tl = tileLen;
    SlotInfo si;
    si = {EqType::Head, 0, 0, "머리", mOffsetX + tl * 2, mOffsetY + tl};
    mSlotInfos.insert({EqType::Head, si});

    si.mEqType = EqType::Back, si.mInfo = "등"; 
    si.x = mOffsetX + tl * 0.7; si.mId++;
    mSlotInfos.insert({EqType::Back, si});

    si.mEqType = EqType::Weapon, si.mInfo = "무기1", 
    si.x = mOffsetX + tl * 4.6; si.mId++;
    mSlotInfos.insert({EqType::Weapon, si});

    si.mEqType = EqType::Torso, si.mInfo = "몸통", 
    si.x = mOffsetX + tl * 2; si.y = mOffsetY + tl * 2 + 32; si.mId++;
    mSlotInfos.insert({EqType::Torso, si});

    si.mEqType = EqType::Hand, si.mInfo = "손1", 
    si.x = mOffsetX + tl * 0.7; si.mId++;
    mSlotInfos.insert({EqType::Hand, si});

    si.mEqType = EqType::Hand, si.mInfo = "손2", 
    si.x = mOffsetX + tl * 3.3; si.mId++;
    mSlotInfos.insert({EqType::Hand, si});

    si.mEqType = EqType::Weapon, si.mInfo = "무기2", 
    si.x = mOffsetX + tl * 4.6; si.mId++;
    mSlotInfos.insert({EqType::Weapon, si});

    si.mEqType = EqType::Leg, si.mInfo = "다리",
    si.x = mOffsetX + tl * 2; si.y = mOffsetY + tl * 3 + 64; si.mId++;
    mSlotInfos.insert({EqType::Leg, si});

    si.mEqType = EqType::Foot, si.mInfo = "발1",
    si.x = mOffsetX + tl * 0.7; si.y = mOffsetY + tl * 4 + 96; si.mId++;
    mSlotInfos.insert({EqType::Foot, si});

    si.mEqType = EqType::Foot, si.mInfo = "발2", 
    si.x = mOffsetX + tl * 3.3; si.mId++;
    mSlotInfos.insert({EqType::Foot, si});
}

void InventoryUI::Activate(Pawn *p)
{
    mCanHandleEvent = true;
    mIsRender = true;
    mIsRenderUpdate = true;

    mGc->mUim->mCanHandleToolTip = true;
    mGc->mUim->mToolTipGrid = mGrid;
}

void InventoryUI::Deactivate()
{
    mCanHandleEvent = false;
    mIsRender = false;

    mGc->mUim->mToolTipGrid = mGc->mMapm->mCurrentMap;
}

void InventoryUI::UpdateEqToolTip(SlotInfo& si)
{
    EntityUtil eu;
    Pawn* p = mGc->mObjm->mEntm->mFocusedPc;

    //엔티티가 실제로 뭘 장비했는지 확인한다.
    //슬롯의 장비 아이디와 현재 장비 아이디를 조회
    Equipment* eq = eu.GetEquipment(p, si.mEqId);
    
    ToolTip* tt = mToolTip;
    if (mPrevEqIdx != si.mId) {
        mPrevEqIdx = si.mId;
 
        LoadEqToolTip(si);
    }

    tt->mIsRender = true;
    tt->SetRefInfo(si.x, si.y, mGrid->mTileLen, mGrid->mTileLen);
    mGc->mUim->mCharacterSheet->mIsRenderUpdate = true;
    tt->mTui->mTotalHeight = 0; tt->mTui->mTotalWidth = 0;
}

void InventoryUI::LoadEqToolTip(SlotInfo &si)
{
    EntityUtil eu;
    Pawn* p = mGc->mObjm->mEntm->mFocusedPc;
    ToolTip* tt = mToolTip;
    Equipment* eq = eu.GetEquipment(p, si.mEqId);

    tt->ClearContent();
    tt->mTui->AddWord(TTFWord(si.mInfo, System::kTc, System::sFont));
    tt->mTui->AddWord(TTFWord(System::sFont, TextType::NewLine));
    tt->mTui->AddWord(TTFWord("equipment id: " + std::to_string(si.mEqId), System::kTc, System::sFont));
    tt->mTui->AddWord(TTFWord(System::sFont, TextType::NewLine));
    
    //장비가 있을 경우에만 정보를 푸시함
    if (eq) {
        tt->mTui->AddWord(TTFWord(eq->mName, System::kTc, System::sFont));
        tt->mTui->AddWord(TTFWord(System::sFont, TextType::NewLine));
    }
    
}

void InventoryUI::HandleEvent(SDL_Event &e, float mx, float my)
{
    if (!mCanHandleEvent) return;
    mMouseIn = false;

    for (auto siPair : mSlotInfos) {
        HandleEqSlotEvent(e, siPair.second, mx, my);
    }

    Pawn* p = static_cast<Pawn*>(mGc->mObjm->mEntm->mFocusedEnt);
    for (auto itemPair : p->mInventory) {
        HandleItemEvent(e, itemPair.second, mx, my);
    }

    if (!mMouseIn) {
        mToolTip->mIsRender = false;
        mGc->mUim->mCharacterSheet->mIsRenderUpdate = true;
    }
}

void InventoryUI::HandleEqSlotEvent(SDL_Event &e, SlotInfo si, float mx, float my)
{
    int tl = mGrid->mTileLen;

    Math mth;
    bool isIn = mth.IsPointInSquare(mx, my, (float) si.x, (float) si.y, (float) tl, (float) tl);
    if (!isIn) return;
    //마우스 오버
    mMouseIn = true;

    Pawn* p = static_cast<Pawn*>(mGc->mObjm->mEntm->mFocusedEnt);
    UpdateEqToolTip(si);
    
    //클릭시
    if (e.type != SDL_EVENT_MOUSE_BUTTON_DOWN || e.button.button != SDL_BUTTON_LEFT) return;
    if (si.mEqId == 0) return; //참조 아이디가 없는 상태면 리턴한다.
    // mGc->mObjm->mEntm->UnequipItem(*mGc, p, si.mEqId);
    EntityUtil eu;
    ItemMenu* im = mGc->mUim->mItemMenu;
    im->UpdatePos(mX, mY, si.x, si.y, mGrid->mTileLen);
    im->Update(eu.GetEquipment(p, si.mEqId), true);
    im->Activate();
}

void InventoryUI::HandleItemEvent(SDL_Event &e, Item* item, float mx, float my)
{
    MapHelper mh; Math mth;
    Point p = mh.GetPosPoint(item->mTileId, mGrid);

    int tl = mGrid->mTileLen;
    bool isIn = mth.IsPointInSquare(mx, my, 
        (float) mGrid->mOffsetX + p.mX * tl,
        (float) mGrid->mOffsetY + p.mY * tl,
        (float) tl, (float) tl
    );
    if (!isIn) return;
    //마우스오버

    if (e.type != SDL_EVENT_MOUSE_BUTTON_DOWN || e.button.button != SDL_BUTTON_LEFT) return;

    ItemMenu* im = mGc->mUim->mItemMenu;
    Pawn* fp = mGc->mObjm->mEntm->mFocusedPc;

    if (item->mType == ItemType::Equipment) {
        Equipment* eq = static_cast<Equipment*>(item);
        im->UpdatePos(mGrid, mX, mY, p.mX, p.mY);
        im->Update(eq, false);
        im->Activate();        
    }
    else if (item->mType == ItemType::Consumable) {
        Consumable* cons = static_cast<Consumable*>(item);
        //여기에 아이템 메뉴 활성화 동작을 삽입..
        im->UpdatePos(mGrid, mX, mY, p.mX, p.mY);
        im->Update(cons);
        im->Activate();
    }
}

void InventoryUI::RenderThings()
{
    ItemHelper ih; MapHelper mh;

    ItemManager* itm = mGc->mObjm->mItm;
    Pawn* focusedP = static_cast<Pawn*> (mGc->mObjm->mEntm->mFocusedEnt);

    mGrid->RenderTiles();
    Texture t;
    t.LoadFromFile("images/ui/frame.png");
    RenderEqSlots(t);

    int tl = mGrid->mTileLen;
    int x {0}; int y {0};
    for (auto itemPair : focusedP->mInventory) {
        Item* item = itemPair.second;
        Point p = mh.GetPosPoint(item->mTileId, mGrid);

        itm->RenderItem(item, 
            (float) p.mX * tl + mGrid->mOffsetX,
            (float) p.mY * tl + mGrid->mOffsetY, 
            (float) tl, (float) tl
        );
    }

    if (mToolTip->mIsRender) {
        mToolTip->RenderThings(
            mToolTip->mRefX + mToolTip->mRefW * 0.5, 
            mToolTip->mRefY + mToolTip->mRefH * 0.5
        );
    }
}

void InventoryUI::RenderEqSlots(Texture& t)
{
    int tl = mGrid->mTileLen;

    for (auto slotPair : mSlotInfos) {
        SlotInfo si = slotPair.second;
        RenderEqSlot(t, si);
    }
}

void InventoryUI::RenderEqSlot(Texture &t, SlotInfo si)
{
    int totalH {0};
    int tl = mGrid->mTileLen;
    TextUI tui = TextUI(0.f, 0.f);
    tui.mPadding = 6;
    TTFWord word = TTFWord(si.mInfo, System::kTc, System::sFont);

    t.Render((float) si.x , (float) si.y, nullptr, (float) tl, (float) tl); //머리
    tui.AddWord(word);
    int rem = tl - word.GetWordWidth(); if (rem < 0) rem = 0;
    
    tui.mX = (float) si.x + rem * 0.5 - tui.mPadding; tui.mY = (float) (si.y + tl);
    tui.RenderWords();

    EntityUtil eu;
    Equipment* eq = eu.GetEquipment(mGc->mObjm->mEntm->mFocusedPc, si.mEqId);
    
    if (eq != nullptr) {
        mGc->mObjm->mItm->RenderItem(eq, si.x, si.y, tl, tl);
    }
}

CharacterSkillUI::CharacterSkillUI(int x, int y, int w, int h)
{
    mX = x; mY = y; mW = w; mH = h;

    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
    for (int i = 0; i < h / 40; i ++) {
        FramedTUI* skill = new FramedTUI(x, y + i * 40, mW * 0.5, 40);
        skill->AddWord(TTFWord("-", tc, System::sFont));
        mSkillList.push_back(skill);
    }

    mSkillDesc = new FramedTUI(x + mW * 0.5, y, mW * 0.5, mH);
    mSkillDesc->AddWord(TTFWord("스킬 설명란이다.", tc, System::sFont));
}

void CharacterSkillUI::Activate(Entity* ent)
{
    mCanHandleEvent = true;
    mIsRender = true;
    UpdateUI(ent);
}

void CharacterSkillUI::Deactivate()
{
    mCanHandleEvent = false;
    mIsRender = false;
}

void CharacterSkillUI::HandleEvent(SDL_Event &e, GameContext *gc, float mx, float my)
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

void CharacterSkillUI::HandleSkillListEvent(SDL_Event &e, GameContext *gc, float mx, float my)
{
    Math mth;
    Entity* focused = gc->mObjm->mEntm->mFocusedEnt;
    if (!focused) return;

    for (int i = 0; i < (int) mSkillList.size(); i++) {
        FramedTUI* s = mSkillList[i];
        bool isMouseIn = mth.IsPointInSquare(mx, my, (float) (s->mX), (float) (s->mY), (float) s->mW, (float) s->mH);
        if (!isMouseIn) continue;
        //마우스오버
        UpdateSkillDesc(i, gc);

        if (e.type != SDL_EVENT_MOUSE_BUTTON_UP || !System::sIsLeftMouseClicked) return;
        //클릭
        std::string message = "character sheet: skill list index is " + std::to_string(i);
        SDL_Log(message.c_str());
        if (i >= (int) focused->mSkills.size()) continue;
        Skill* skill = focused->mSkills[i];
        gc->mUim->mQSUI->AddSkill(skill, i);
    }
}

void CharacterSkillUI::UpdateUI(Entity *ent)
{
    int i = 0;
    for (Skill* skill : ent->mSkills) {
        mSkillList[i]->mTui->mTexts[0].mMessage = skill->mName;
        i++;
    }
}

void CharacterSkillUI::UpdateSkillDesc(int idx, GameContext* gc)
{
    Entity* focused = gc->mObjm->mEntm->mFocusedEnt;
    if (!focused) return;
    if (idx >= (int) focused->mSkills.size()) return;

    Skill* tskill = focused->mSkills[idx];

    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};

    gc->mUim->mCharacterSheet->mIsRenderUpdate = true;
    mIsRenderUpdate = true;

    mSkillDesc->ClearText();
    mSkillDesc->AddWord(TTFWord(tskill->mName, tc, System::sFont));
}

void CharacterSkillUI::RenderThings()
{
    for (FramedTUI* skill : mSkillList) {
        skill->Render();
    }
    mSkillDesc->Render();
}

CharacterSheetUI::CharacterSheetUI(int x, int y, int w, int h, GameContext* gc)
{
    mGc = gc;

    mX = x; mY = y; mW = w; mH = h;

    mInvTab = new Button(0, 0, 120, 60, "인벤토리", BtnType::Default);
    mSkillTab = new Button(120, 0, 120, 60, "스킬", BtnType::Default);

    mInvUI = new InventoryUI(x, y, 0, 60, w, h - 60, 64, gc);
    mCsUI = new CharacterSkillUI(0, 60, w, h - 60);

    TextureManager tm;
    mTempTex = tm.CreateTempTexture(System::sRenderer, w, h);
}

void CharacterSheetUI::Activate()
{
    Pawn* pc = mGc->mObjm->mEntm->mFocusedPc;
    if (!mIsRender) mGc->mUim->mUIStack.push_back(this);

    Deactivate();
    mGc->mUim->mCanHandleToolTip = false;
    mGc->mObjm->mItm->mCanHandleEvent = false;
    mGc->mObjm->mEntm->mCanHandleEvent = false;
    mCanHandleEvent = true;
    mIsRender = true;
    mIsRenderUpdate = true;
    if (mTabIdx == CharacterSheetIdx::Skill) mCsUI->Activate(pc);
    if (mTabIdx == CharacterSheetIdx::Inv) mInvUI->Activate(pc);
}

void CharacterSheetUI::Deactivate()
{
    mGc->mUim->mCanHandleToolTip = true;
    mGc->mObjm->mItm->mCanHandleEvent = true;
    mGc->mObjm->mEntm->mCanHandleEvent = true;    
    mCanHandleEvent = false;
    mIsRender = false;
    mCsUI->Deactivate();
    mInvUI->Deactivate();
}

void CharacterSheetUI::HandleEvent(SDL_Event &e, GameContext &gc, float mx, float my)
{
    if (!mCanHandleEvent) return;
    if (gc.mEvCtx->mIsEventHandled) return;
    mx -= (float) mX; my -= (float) mY; //오프셋

    //서브 ui 이벤트 핸들링
    mCsUI->HandleEvent(e, &gc, mx, my);
    mInvUI->HandleEvent(e, mx, my);

    Entity* ent = gc.mObjm->mEntm->mFocusedEnt;
    if (!ent) {
        SDL_Log("character sheet: null focused entity!");
        return;
    }

    //캐릭터 시트 탭 이벤트 핸들링
    Pawn* p = static_cast<Pawn*>(ent);
    if (mInvTab->IsMouseIn(mx, my)) {
        if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            mTabIdx = CharacterSheetIdx::Inv;
            Activate();
        }
    }
    if (mSkillTab->IsMouseIn(mx, my)) {
        if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            mTabIdx = CharacterSheetIdx::Skill;
            Activate();
        }
    }
}

void CharacterSheetUI::RenderThings()
{
    Texture t;
    t.LoadFromFile("images/ui/character_sheet.png");
    t.Render(0.f, 0.f, nullptr, (float) mW, (float) mH);

    mInvTab->Render();
    mSkillTab->Render();
    
    if (mCsUI->mIsRender) mCsUI->RenderThings();
    if (mInvUI->mIsRender) mInvUI->RenderThings();
}

QuickSkillUI::QuickSkillUI(int x, int  y, int w, int h, GameContext& gc)
{
    mX = x; mY = y; mW = w; mH = h;

    TextureManager tm;
    mTempTex = tm.CreateTempTexture(System::sRenderer, mW, mH);

    mGc = &gc;
}

void QuickSkillUI::AddSkill(Skill* skill, int skillIdx)
{
    Entity* focused = mGc->mObjm->mEntm->mFocusedEnt;
    if (!focused) return;
    if (!focused->mIsPawn) return;

    Pawn* p = static_cast<Pawn*> (focused);

    if (p->mQuickSkills.size() > System::sQuickSlotCap) {
        SDL_Log("quick skill ui: cannot add skill, size limit hit");
        return;
    }
    int i = 0; 
    for (auto iter = p->mQuickSkills.begin(); iter != p->mQuickSkills.end();) {
        //삼입하려는 스킬 코드가 이미 퀵슬롯 ui에 있을 경우
        if (*iter == skill->mCode) {
            //해당 친구를 컨테이너에서 삭제하고 리턴한다.
            p->mQuickSkills.erase(iter);
            mSkillIdxs.erase(mSkillIdxs.begin() + i);
            mIsRenderUpdate = true;
            SDL_Log("quick skill ui: erased skill code");
            return;
        }
        iter++; i++;
    }

    p->mQuickSkills.push_back(skill->mCode);
    mSkillIdxs.push_back(skillIdx);
    mIsRenderUpdate = true;
    SDL_Log("quick skill ui: added skill code");
}

void QuickSkillUI::RenderThings()
{
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
        int idx = *std::next(mSkillIdxs.begin(), xPos);
        Skill* skill = gc.mObjm->mEntm->mFocusedEnt->mSkills[idx];

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
            else gc.mUim->mTileHLUI->mHighlight->LoadFromFile("images/ui/highlight.png");

            mGc->mSkm->mIsSkillReady = true; //스킬 사용 준비 완료
        }

        mGc->mSkm->SetSkill(skill); //스킬 객체 캐싱
        mGc->mSkm->SetActor(gc.mObjm->mEntm->mFocusedEnt); //액터 캐싱

        map->mCanHandleEvent = true;
    }
}

void QuickSkillUI::Activate()
{
    mGc->mUim->mUIStack.push_back(this);
    mCanHandleEvent = true;
    mIsRenderUpdate = true;
    mIsRender = true;
    mGc->mMapm->mCurrentMap->mCanHandleEvent = false;
}

void QuickSkillUI::Deactivate()
{
    mCanHandleEvent = false;
    mIsRender = false; //스킬 퀵슬롯 렌더링 안함
    mGc->mMapm->mCurrentMap->mCanHandleEvent = true;
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

void BottomCharacterUI::RenderThings()
{
    Texture t;
    t.LoadFromFile("images/ui/bottom_char_ui.png");
    t.Render(0.f, 0.f, nullptr, (float) mW, (float) mH);
    mMainStat->Render();
    mStatEffect->Render();
}

ItemMenu::ItemMenu(int x, int y, int w, int h, GameContext* gc)
{
    mX = x; mY = y; mW = w; mH = h;
    mGc = gc;

    TextureManager tm;
    mTempTex = tm.CreateTempTexture(System::sRenderer, mW, mH);
}

void ItemMenu::Activate()
{
    std::vector<UI*>& uistack = mGc->mUim->mUIStack;
    
    //상태가 변화했을 때만 스택 동작을 수행한다.
    if (!mIsRender) {
        //스택이 비어있지 않은 경우 이젠 스택 객체의 이벤트 핸들링을 비활성화한다.
        if (!uistack.empty()) uistack.back()->mCanHandleEvent = false;
        uistack.push_back(this);
        SDL_Log("item menu stacked");
    }
    mCanHandleEvent = true;
    mIsRender = true;
    mIsRenderUpdate = true;
}

void ItemMenu::Deactivate()
{
    SDL_Log("deactivated item menu");
    mCanHandleEvent = false;
    mIsRender = false;
}

void ItemMenu::ClearButtons()
{
    for (Button* btn : mButtons) {
        delete btn;
        btn = nullptr;
    }
    mButtons.clear();
}

bool ItemMenu::HandleEvent(SDL_Event &e, float mx, float my)
{
    if (mGc->mEvCtx->mIsEventHandled) return;
    if (!mCanHandleEvent) return;
    Math mth;

    bool isConsumed {false};

    if (e.type != SDL_EVENT_MOUSE_BUTTON_DOWN || e.button.button != SDL_BUTTON_LEFT) return isConsumed;

    EntityManager* entm = mGc->mObjm->mEntm;
    UIManager* uim = mGc->mUim;

    for (int i = 0; i < (int) mButtons.size(); i++) {
        bool isIn = mButtons[i]->IsMouseIn(mx, my);
        
        if (isIn) {
            if (mBtnIdxs[i] == ItemMenuBtnIdx::Use) {
                SDL_Log("item menu: use");
            }
            if (mBtnIdxs[i] == ItemMenuBtnIdx::Equip) {
                SDL_Log("item menu: equip");
                Equipment* eq = static_cast<Equipment*>(mItem);
                entm->EquipItem(*mGc, entm->mFocusedPc, eq);
            }
            if (mBtnIdxs[i] == ItemMenuBtnIdx::Unequip) {
                SDL_Log("item menu: unequip");
                entm->UnequipItem(*mGc, entm->mFocusedPc, mItem->mId);
            }
            if (mBtnIdxs[i] == ItemMenuBtnIdx::Modify) {
                SDL_Log("item menu: modify");
            }
            if (mBtnIdxs[i] == ItemMenuBtnIdx::Examine) {
                SDL_Log("item menu: examine");
            }
            if (mBtnIdxs[i] == ItemMenuBtnIdx::Dispose) {
                SDL_Log("item menu: dispose");
            }
            uim->PopBackUI();
            isConsumed = true;
            mGc->mEvCtx->mIsEventHandled = true;
        }

    }

    return isConsumed;
}

void ItemMenu::UpdatePos(int baseX, int baseY, int x, int y, int tl)
{
    mX = baseX + x + tl + 20;
    mY = baseY + y + 20;
}

void ItemMenu::UpdatePos(Grid *grid, int baseX, int baseY, int pointX, int pointY)
{
    int tl = grid->mTileLen;
    mX = baseX + grid->mOffsetX + (pointX + 1) * tl + 20;
    mY = baseY + grid->mOffsetY + (pointY) * tl + 20;
}

void ItemMenu::Update(Consumable *cons)
{
    mItem = cons;

    mBtnIdxs.clear();
    ClearButtons();

    mButtons.push_back(new Button(mX, mY, mW, 40, "사용하기", BtnType::Default));
    mBtnIdxs.push_back(ItemMenuBtnIdx::Use);
    mButtons.push_back(new Button(mX, mY + 40, mW, 40, "확인하기", BtnType::Default));
    mBtnIdxs.push_back(ItemMenuBtnIdx::Examine);
    mButtons.push_back(new Button(mX, mY + 80, mW, 40, "버리기", BtnType::Default));
    mBtnIdxs.push_back(ItemMenuBtnIdx::Dispose);
}

void ItemMenu::Update(Equipment *eq, bool isEquipped)
{
    mItem = eq;

    mBtnIdxs.clear();
    ClearButtons();

    if (isEquipped) {
        mButtons.push_back(new Button(mX, mY, mW, 40, "장비해제", BtnType::Default));
        mBtnIdxs.push_back(ItemMenuBtnIdx::Unequip);
    }
    else {
        mButtons.push_back(new Button(mX, mY, mW, 40, "장비하기", BtnType::Default));
        mBtnIdxs.push_back(ItemMenuBtnIdx::Equip);
    }
    mButtons.push_back(new Button(mX, mY + 40, mW, 40, "개조하기", BtnType::Default));
    mBtnIdxs.push_back(ItemMenuBtnIdx::Modify);
    mButtons.push_back(new Button(mX, mY + 80, mW, 40, "확인하기", BtnType::Default));
    mBtnIdxs.push_back(ItemMenuBtnIdx::Examine);
    mButtons.push_back(new Button(mX, mY + 120, mW, 40, "버리기", BtnType::Default));
    mBtnIdxs.push_back(ItemMenuBtnIdx::Dispose);
}

void ItemMenu::RenderThings()
{
    Texture t;
    SDL_SetTextureScaleMode(t.mTexture, SDL_SCALEMODE_NEAREST);
    t.LoadFromFile("images/ui/item_menu.png");
    t.Render(0.f, 0.f, nullptr, (float) mW, (float) mH);

    for (Button* btn : mButtons) {
        btn->RenderThings(btn->mX  - mX, btn->mY - mY);
    }
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
    TTFWord word = TTFWord(message, c, System::sFont);
    
    //메세지 총 길이와 넓이를 넘어간 횟수를 구한다.
    int ln = word.GetWordWidth();
    int count = ln / (mBody->mW - mBody->mPadding * 2);

    //텍스트 총 높이 연산후 캐싱
    for (int i = 0; i < count; i++) {        
        mTotalH += TTF_GetFontHeight(System::sFont);
        mTotalH += mBody->mTui->mLineSpacing;
    }

    mTotalH += TTF_GetFontHeight(System::sFont);
    mTotalH += mBody->mTui->mLineSpacing;

    //로그창 높이를 초과한 경우 텍스트 비우기
    if (mTotalH > mH - mBody->mPadding * 2 - TTF_GetFontHeight(System::sFont)) {
        mBody->ClearText();
        mTotalH = 0;
    }

    mBody->AddWordAndProcess(word);
    mBody->AddWord(TTFWord(System::sFont, TextType::NewLine));
    
    mIsRenderUpdate = true;
}

void LogUI::RenderThings()
{
    Texture* bg = new Texture("images/ui/log.png");
    bg->Render(0.f, 0.f, nullptr, (float) mW, (float) mH);
    mBody->Render();
}

void UIHelper::AddEntityData(TextUI *tui, Entity *ent)
{
    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
    SDL_Color yellow = {0xB0, 0xB0, 0x40, 0xFF};
    SDL_Color white = {0xF0, 0xF0, 0xF0, 0xFF};
    SDL_Color red = {0xB0, 0x40, 0x40, 0xFF};
    SDL_Color blue = {0x40, 0x40, 0xB0, 0xFF};
    if (ent->mDemeanor == Demeanor::Hostile) tc = {0xB0, 0x00, 0x00, 0xFF};
    if (ent->mDemeanor == Demeanor::Neutral) tc = yellow;

    TTFWord name = TTFWord(ent->mName, tc, System::sFont);

    StatHelper sh;

    tui->AddWord(name);
    tui->AddWord(TTFWord(System::sFont, TextType::NewLine));
    tui->AddWord(TTFWord("HP:", red, System::sFont));
    tui->AddWord(TTFWord(System::sFont, TextType::Space));
    tui->AddWord(TTFWord(std::to_string(ent->mCurHp), white, System::sFont));
    std::string maxHp = "/" + std::to_string(sh.GetMaxHp(ent));
    tui->AddWord(TTFWord(maxHp, white, System::sFont));
    tui->AddWord(TTFWord(System::sFont, TextType::NewLine));

    tui->AddWord(TTFWord("AP:", blue, System::sFont));
    tui->AddWord(TTFWord(System::sFont, TextType::Space));
    tui->AddWord(TTFWord(std::to_string(ent->mCurAp), white, System::sFont));
    std::string maxAp = "/" + std::to_string(sh.GetMaxAp(ent));
    tui->AddWord(TTFWord(maxAp, white, System::sFont));
    tui->AddWord(TTFWord(System::sFont, TextType::NewLine));
}


