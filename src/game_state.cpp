#include "pch.h"

#include "game_state.h"
#include "game_object.h"
#include "system.h"
#include "map.h"
#include "entity.h"
#include "item.h"
#include "city.h"
#include "render.h"
#include "square.h"
#include "ui.h"
#include "text.h"

GameStateManager::GameStateManager()
{
    mIs = new IntroState();
    mOms = new OverMapState();
    mCvs = new CityViewState();
}

void GameStateManager::SetCurrentState(UIManager& uim, ObjectManager& objm)
{
    //상태변화 플래그가 거짓이면 현재 상태를 리턴
    if (!mIsStateChange) return;
    //거짓이 아니면
    mIsStateChange = false; //플래그 초기화

    //다음상태가 널인지를 확인.
    if (mNextState != nullptr) {
        //다음상태에 뭔가가 할당되어 있다.

        mCurrentState->Exit(uim, objm); //현재 상태 탈출 메서드
        mNextState->Enter(uim, objm); //다음 상태 진입 메서드

        //다음 상태를 임시 객체에 할당해줌
        GameState* tempState = mNextState;
        mNextState = nullptr; //초기화

        mCurrentState = tempState; //현재 상태를 실제로 바꿈
    }
    else {
        //다음상태가 널이면 그냥 현재 상태를 반환함.
        return;
    }
}

void IntroState::Enter(UIManager& uim, ObjectManager& objm)
{
    SDL_Log("enter intro");
    uim.uiMap["introUI"] = new Button(new Square(System::sWindowWidth/2 - 100, System::sWindowHeight/2 - 25, 200, 50), "시작", BtnType::OverMap);
}

void IntroState::Exit(UIManager& uim, ObjectManager& objm)
{
    SDL_Log("exit intro");
    uim.DestroyUIs();
}

void IntroState::Update(UIManager& uim, ObjectManager& objm, GameStateManager& gsm)
{

}

void IntroState::HandleEvent(SDL_Event& e,UIManager& uim, ObjectManager& objm, GameStateManager& gsm, float mouseX, float mouseY)
{
    for (auto ui : uim.uiMap) {
        ui.second->HandleEvent(e, gsm, mouseX, mouseY);
    }
}

void IntroState::Render(RenderManager& rend, UIManager& uim, ObjectManager& objm)
{
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(System::sRenderer);

    uim.RenderUIs();
    rend.RenderFps();

    SDL_RenderPresent(System::sRenderer);
}

void OverMapState::Enter(UIManager& uim, ObjectManager& objm)
{
    SDL_Log("enter overmap");

    //렌더링 플래그 일시 true
    //맵 렌더링 플래그
    objm.mMap->mIsMapUpdate = true;
    //오버맵에서 팀 렌더링 플래그
    objm.mTeamm->mIsTeamUpdate = true;

    //ui 생성
    //사이드바
    uim.uiMap["titleButton"] = new Button(new Square(10, 10 + uim.mTopPanelH, 100, 50), "타이틀로", BtnType::Title);
    uim.uiMap["cityViewButton"] = new Button(new Square(10, 70 + uim.mTopPanelH, 100, 50), "도시", BtnType::City);

    uim.ftuiMap["babo"] = new FramedTUI(100, 100, 200, 400);
    FramedTUI* ftui = uim.ftuiMap["babo"];

    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
    ftui->mTui->ProcessAndAddText("hi my name is babo and i would like to die 그리고 응애에요.", tc, System::sFont);

    //탑 바
    tc = {0x00, 0xD0, 0x00, 0xFF};
    uim.uiMap["turnIcon"] = new IconUI(10, 0, 60, 60, "images/icons/turn.png");
    uim.uiMap["turnText"] = new TextUI(70, 0);

    uim.uiMap["supplyIcon"] = new IconUI(130, 0, 60, 60, "images/icons/supply.png");
    uim.uiMap["supplyText"] = new TextUI(190, 0);
}

void OverMapState::Exit(UIManager& uim, ObjectManager& objm)
{
    SDL_Log("exit overmap");
    uim.DestroyUIs(); //ui파괴
}

void OverMapState::Update(UIManager& uim, ObjectManager& objm, GameStateManager& gsm)
{
    uim.UpdateMapToolTip(objm.mMap);
}

void OverMapState::HandleEvent(SDL_Event& e, UIManager& uim, ObjectManager& objm, GameStateManager& gsm, float mouseX, float mouseY)
{
    for (auto ui : uim.uiMap) {
        ui.second->HandleEvent(e, gsm, mouseX, mouseY);
    }

    for (auto ftui : uim.ftuiMap) {
        SDL_Log("handling ftui");
        ftui.second->HandleEvent(e, gsm, mouseX, mouseY);
    }

    uim.HandleMapToolTipEvent(e, gsm, objm.mMap, mouseX, mouseY);
}

void OverMapState::Render(RenderManager& rend, UIManager& uim, ObjectManager& objm)
{
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(System::sRenderer);
    
    SDL_SetRenderLogicalPresentation(System::sRenderer, 1280, 720, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    //ui패널 렌더링
    Square topPanel = Square(0, 0, System::sWindowWidth, uim.mTopPanelH);
    topPanel.Render();

    //맵 렌더링
    objm.mMap->RenderOnUpdate();
    //엔티티 렌더링
    objm.mTeamm->RenderOnUpdate();

    //ui들 렌더링
    uim.RenderUIs();
    //툴팁 렌더링
    uim.mToolTip->RenderOnUpdate();

    rend.RenderFps();

    SDL_RenderPresent(System::sRenderer);
}

void CityViewState::Enter(UIManager &uim, ObjectManager &objm)
{
    SDL_Log("enter city view");

    //도시 렌더링 플래그
    objm.mCity->mCityMap->mIsMapUpdate = true;

    //사이드바
    uim.uiMap["titleButton"] = new Button(new Square(10, 10 + uim.mTopPanelH, 100, 50), "타이틀로", BtnType::Title);
    uim.uiMap["overMapButton"] = new Button(new Square(10, 70 + uim.mTopPanelH, 100, 50), "오버맵", BtnType::OverMap);

    //탑 바
    uim.uiMap["turnIcon"] = new IconUI(10, 0, 60, 60, "images/icons/turn.png");

    TextUI* turnTui = new TextUI(70, 0);
    SDL_Color tc = {0x00, 0xD0, 0x00, 0xFF};
    turnTui->mTexts.push_back(TTFWord("0", tc, System::sFont40));

    uim.uiMap["turnText"] = turnTui;

    uim.uiMap["supplyIcon"] = new IconUI(130, 0, 60, 60, "images/icons/supply.png");
    uim.uiMap["supplyText"] = new TextUI(190, 0);
}

void CityViewState::Exit(UIManager &uim, ObjectManager &objm)
{
    SDL_Log("exit city view");
    uim.DestroyUIs(); //ui파괴
}

void CityViewState::Update(UIManager &uim, ObjectManager &objm, GameStateManager &gsm)
{
    uim.UpdateMapToolTip(objm.mCity->mCityMap);
}

void CityViewState::HandleEvent(SDL_Event &e, UIManager &uim, ObjectManager &objm, GameStateManager &gsm, float mouseX, float mouseY)
{
    for (auto ui : uim.uiMap) {
        ui.second->HandleEvent(e, gsm, mouseX, mouseY);
    }

    uim.HandleMapToolTipEvent(e, gsm, objm.mCity->mCityMap, mouseX, mouseY);
}

void CityViewState::Render(RenderManager &rend, UIManager &uim, ObjectManager &objm)
{
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(System::sRenderer);

    
    //ui패널 렌더링
    Square topPanel = Square(0, 0, System::sWindowWidth, uim.mTopPanelH);
    topPanel.Render();
    //ui렌더링
    uim.RenderUIs();

    //도시 맵 렌더링
    objm.mCity->mCityMap->RenderOnUpdate();
    //툴팁 렌더링
    uim.mToolTip->RenderOnUpdate();

    rend.RenderFps();

    SDL_RenderPresent(System::sRenderer);
}
