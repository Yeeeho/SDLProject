#include "pch.h"

#include "game_state.h"
#include "turn.h"
#include "scenario.h"
#include "game_object.h"
#include "system.h"
#include "camera.h"
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
    mSms = new SubMapState();

    mScm = new ScenarioManager();
    mTms = new TurnManagers();
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

        mCurrentState->Exit(uim, objm, *this); //현재 상태 탈출 메서드
        mNextState->Enter(uim, objm, *this); //다음 상태 진입 메서드

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

GameState::GameState()
{
}

void IntroState::Enter(UIManager& uim, ObjectManager& objm, GameStateManager& gsm)
{
    SDL_Log("enter intro");
    uim.uiMap["debugBtn"] = new Button(new Square(System::sWindowWidth/2 - 100, System::sWindowHeight/2 - 25, 200, 50), "디버그", BtnType::OverMap);
    uim.uiMap["newGameBtn"] = new Button(new Square(System::sWindowWidth/2 - 100, System::sWindowHeight/2 - 150, 200, 50), "새 게임", BtnType::NewGame);
}

void IntroState::Exit(UIManager& uim, ObjectManager& objm, GameStateManager& gsm)
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
        ui.second->HandleEvent(e, gsm, objm, mouseX, mouseY);
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

void OverMapState::Enter(UIManager& uim, ObjectManager& objm, GameStateManager& gsm)
{
    SDL_Log("enter overmap");

    //시나리오 로딩

    //렌더링 플래그 일시 true
    //맵 렌더링 플래그
    objm.mMap->mIsMapUpdate = true;
    //오버맵에서 팀 렌더링 플래그
    objm.mTeamm->mIsTeamUpdate = true;

    //ui 생성
    //사이드바
    uim.uiMap["titleButton"] = new Button(new Square(10, 10 + uim.mTopPanelH, 100, 50), "타이틀로", BtnType::Title);
    uim.uiMap["cityViewButton"] = new Button(new Square(10, 70 + uim.mTopPanelH, 100, 50), "도시", BtnType::City);
    //턴 종료 버튼 타입 변경
    uim.mTurnOverBtn->mType = BtnType::OverMapTurnOver;

    uim.ftuiMap["babo"] = new FramedTUI(100, 100, 200, 400);
    FramedTUI* ftui = uim.ftuiMap["babo"];

    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
    ftui->mTui->ProcessAndAddText("hi my name is babo and i would like to die 그리고 응애에요.", tc, System::sFont);

    //탑 바
    uim.InitTopBar();
}

void OverMapState::Exit(UIManager& uim, ObjectManager& objm, GameStateManager& gsm)
{
    SDL_Log("exit overmap");
    uim.DestroyUIs(); //ui파괴
}

void OverMapState::Update(UIManager& uim, ObjectManager& objm, GameStateManager& gsm)
{
    gsm.mScm->Update(uim, objm, gsm);

    objm.mMap->mCam->Move();
    uim.UpdateMapToolTip(objm.mMap);
}

void OverMapState::HandleEvent(SDL_Event& e, UIManager& uim, ObjectManager& objm, GameStateManager& gsm, float mouseX, float mouseY)
{
    objm.mMap->mCam->HandleEvent(e);

    uim.HandleUIEvent(e, gsm, objm, mouseX, mouseY);
    uim.HandleMapUIEvent(e, gsm, objm.mMap, mouseX, mouseY);
}

void OverMapState::Render(RenderManager& rend, UIManager& uim, ObjectManager& objm)
{
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(System::sRenderer);
    
    SDL_SetRenderLogicalPresentation(System::sRenderer, 1280, 720, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    //맵 렌더링
    objm.mMap->RenderOnUpdate();
    //팀 렌더링
    objm.mTeamm->RenderOnUpdate(objm.mMap);

    //ui들 렌더링
    uim.mToolTip->RenderOnUpdate();    //툴팁 렌더링
    uim.RenderMapUIs(objm.mMap);

    uim.RenderUIs();
    //턴종료 버튼
    if (!uim.mDialogueUI->mIsRender) uim.mTurnOverBtn->RenderOnUpdate();
    rend.RenderFps();

    SDL_RenderPresent(System::sRenderer);
}

void SubMapState::Enter(UIManager& uim, ObjectManager& objm, GameStateManager& gsm) 
{
    SDL_Log("enter submap");
    
    gsm.mTms->mSmtm.EnterMap(objm.mSubMap);
    objm.mSubMap->mIsMapUpdate = true;

    uim.mTurnOverBtn->mType = BtnType::SubMapTurnOver;

    uim.InitTopBar();

}

void SubMapState::Exit(UIManager &uim, ObjectManager &objm, GameStateManager& gsm)
{
    SDL_Log("exit submap");
    gsm.mScm->ClearThings(objm);

    uim.DestroyUIs();
}

void SubMapState::Update(UIManager &uim, ObjectManager &objm, GameStateManager &gsm)
{
    gsm.mScm->Update(uim, objm, gsm);
    gsm.mTms->mSmtm.Update();

    objm.mSubMap->mCam->Move();

    uim.UpdateMapToolTip(objm.mSubMap);
}

void SubMapState::HandleEvent(SDL_Event &e, UIManager &uim, ObjectManager &objm, GameStateManager &gsm, float mouseX, float mouseY)
{
    uim.mDialogueUI->HandleEvent(e, gsm, mouseX, mouseY);

    objm.mEntm->HandleEvent(e, uim, objm, objm.mSubMap, mouseX, mouseY);

    objm.mSubMap->mCam->HandleEvent(e);
    objm.mSubMap->HandleEvent(e, uim, objm, mouseX, mouseY);

    uim.HandleUIEvent(e, gsm, objm, mouseX, mouseY);
    uim.HandleMapUIEvent(e, gsm, objm.mSubMap, mouseX, mouseY);
}

void SubMapState::Render(RenderManager &rend, UIManager &uim, ObjectManager &objm)
{
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(System::sRenderer);
    
    SDL_SetRenderLogicalPresentation(System::sRenderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);
    
    //맵 렌더링
    objm.mSubMap->RenderOnUpdate();
    //엔티티 렌더링
    objm.mEntm->RenderEntities(objm.mSubMap);
    
    //ui들 렌더링
    uim.mFocusIcon->RenderByCam(objm.mSubMap->mCam);
    uim.RenderMapUIs(objm.mSubMap);
    uim.RenderUIs();

    uim.mDialogueUI->RenderOnUpdate();
    if (!uim.mDialogueUI->mIsRender) uim.mTurnOverBtn->RenderOnUpdate();

    SDL_RenderPresent(System::sRenderer);
}

void CityViewState::Enter(UIManager &uim, ObjectManager &objm, GameStateManager& gsm)
{
    SDL_Log("enter city view");

    //시나리오 로딩

    //도시 렌더링 플래그
    objm.mCity->mCityMap->mIsMapUpdate = true;

    //사이드바
    uim.uiMap["titleButton"] = new Button(new Square(10, 10 + uim.mTopPanelH, 100, 50), "타이틀로", BtnType::Title);
    uim.uiMap["overMapButton"] = new Button(new Square(10, 70 + uim.mTopPanelH, 100, 50), "오버맵", BtnType::OverMap);

    //탑 바
    uim.InitTopBar();
    uim.InitUIs();

    uim.mTurnOverBtn->mType = BtnType::CityMapTurnOver;
}

void CityViewState::Exit(UIManager &uim, ObjectManager &objm, GameStateManager& gsm)
{
    SDL_Log("exit city view");
    uim.DestroyUIs(); //ui파괴
}

void CityViewState::Update(UIManager &uim, ObjectManager &objm, GameStateManager &gsm)
{
    gsm.mScm->Update(uim, objm, gsm);

    uim.UpdateMapToolTip(objm.mCity->mCityMap);
}

void CityViewState::HandleEvent(SDL_Event &e, UIManager &uim, ObjectManager &objm, GameStateManager &gsm, float mouseX, float mouseY)
{
    uim.HandleUIEvent(e, gsm, objm, mouseX, mouseY);

    uim.HandleMapUIEvent(e, gsm, objm.mCity->mCityMap, mouseX, mouseY);
}

void CityViewState::Render(RenderManager &rend, UIManager &uim, ObjectManager &objm)
{
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(System::sRenderer);

    //도시 맵 렌더링
    objm.mCity->mCityMap->RenderOnUpdate();
    //툴팁 렌더링
    uim.mToolTip->RenderOnUpdate();
    uim.RenderUIs();    //ui렌더링
    uim.RenderMapUIs(objm.mCity->mCityMap);
    //턴 종료 버튼
    if (!uim.mDialogueUI->mIsRender) uim.mTurnOverBtn->RenderOnUpdate();

    rend.RenderFps();
    SDL_RenderPresent(System::sRenderer);
}

