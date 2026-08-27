#include "pch.h"

#include "system/game_random.h"
#include "system/io.h"
#include "game_context.h"
#include "game_state.h"
#include "turn.h"
#include "skill/skill.h"
#include "scenario.h"
#include "game_object.h"
#include "system/system.h"
#include "camera.h"
#include "map.h"
#include "entity.h"
#include "item/item.h"
#include "item/item_manager.h"
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
}

void GameStateManager::SetCurrentState(GameContext& gc)
{
    //상태변화 플래그가 거짓이면 현재 상태를 리턴
    if (!mIsStateChange) return;
    //거짓이 아니면
    mIsStateChange = false; //플래그 초기화

    //다음상태가 널인지를 확인.
    if (mNextState != nullptr) {
        //다음상태에 뭔가가 할당되어 있다.

        mCurrentState->Exit(gc); //현재 상태 탈출 메서드
        mNextState->Enter(gc); //다음 상태 진입 메서드

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

void IntroState::Enter(GameContext& gc)
{
    SDL_Log("enter intro");
    gc.mUim->uiMap["debugBtn"] = new Button(System::sWindowWidth/2 - 100, System::sWindowHeight/2 - 25, 200, 50, "디버그", BtnType::OverMap);
    gc.mUim->uiMap["newGameBtn"] = new Button(System::sWindowWidth/2 - 100, System::sWindowHeight/2 - 150, 200, 50, "새 게임", BtnType::NewGame);
}

void IntroState::Exit(GameContext& gc)
{
    SDL_Log("exit intro");
    gc.mUim->DestroyUIs();
}

void IntroState::Update(GameContext& gc)
{
}

void IntroState::HandleEvent(SDL_Event& e, GameContext& gc, float mouseX, float mouseY)
{
    for (auto ui : gc.mUim->uiMap) {
        ui.second->HandleEvent(e, gc, mouseX, mouseY);
    }
}

void IntroState::Render( GameContext& gc)
{
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(System::sRenderer);

    gc.mUim->RenderUIs();
    gc.mRenderM->RenderFps();

    SDL_RenderPresent(System::sRenderer);
}

void OverMapState::Enter(GameContext& gc)
{
    SDL_Log("enter overmap");
    Map* map = gc.mMapm->mOverMap;

    gc.mTurnm->Enter(map);
    gc.mUim->mToolTipGrid = map; 
    //시나리오 로딩

    gc.mMapm->mCurrentMap = map;
    //렌더링 플래그 일시 true
    //맵 렌더링 플래그
    gc.mMapm->mOverMap->mIsMapUpdate = true;
    //오버맵에서 팀 렌더링 플래그
    gc.mObjm->mTeamm->mIsTeamUpdate = true;

    //ui 생성
    //사이드바
    gc.mUim->uiMap["titleButton"] = new Button(10, 10 + gc.mUim->mTopPanelH, 100, 50, "타이틀로", BtnType::Title);
    gc.mUim->uiMap["cityViewButton"] = new Button(10, 70 + gc.mUim->mTopPanelH, 100, 50, "도시", BtnType::City);
    //턴 종료 버튼 타입 변경
    gc.mUim->mTurnOverBtn->mType = BtnType::OverMapTurnOver;

    //탑 바
    gc.mUim->InitTopBar();
}

void OverMapState::Exit(GameContext& gc)
{
    SDL_Log("exit overmap");
    gc.mUim->DestroyUIs(); //ui파괴
}

void OverMapState::Update(GameContext& gc)
{
    gc.mScm->Update(gc);

    gc.mMapm->mOverMap->mCam->Move();
    gc.mUim->UpdateGridToolTip(gc.mUim->mToolTipGrid);
}

void OverMapState::HandleEvent(SDL_Event& e, GameContext& gc, float mouseX, float mouseY)
{
    gc.mMapm->mOverMap->mCam->HandleEvent(e);

    gc.mUim->HandleUIEvent(e, gc, mouseX, mouseY);
    gc.mUim->HandleMapUIEvent(e, gc, gc.mMapm->mOverMap, mouseX, mouseY);
}

void OverMapState::Render(GameContext& gc)
{
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(System::sRenderer);
    
    SDL_SetRenderLogicalPresentation(System::sRenderer, 1280, 720, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    //맵 렌더링
    gc.mMapm->mOverMap->RenderOnUpdate();
    //아이템 렌더링
    gc.mObjm->mItm->Render();
    //팀 렌더링
    gc.mObjm->mTeamm->RenderOnUpdate(gc.mMapm->mOverMap);

    //ui들 렌더링
    gc.mUim->mToolTip->Render();    //툴팁 렌더링
    gc.mUim->RenderMapUIs(gc.mMapm->mOverMap);

    gc.mUim->RenderUIs();
    //턴종료 버튼
    if (!gc.mUim->mDialogueUI->mIsRender) gc.mUim->mTurnOverBtn->Render();
    gc.mRenderM->RenderFps();

    SDL_RenderPresent(System::sRenderer);
}

void SubMapState::Enter(GameContext& gc) 
{
    SDL_Log("enter submap");

    //debug
    IOHelper::MakeFile("save/test.json", "11");

    Map* map = gc.mMapm->mSubMap;
    gc.mMapm->mCurrentMap = map;
    gc.mUim->mToolTipGrid = map;
    gc.mTurnm->Enter(map);

    gc.mMapm->mSubMap->mIsMapUpdate = true;

    gc.mUim->mTurnOverBtn->mType = BtnType::SubMapTurnOver;

    gc.mUim->InitTopBar();

}

void SubMapState::Exit(GameContext& gc)
{
    SDL_Log("exit submap");
    gc.mScm->ClearThings(gc);

    gc.mUim->DestroyUIs();
}

void SubMapState::Update(GameContext& gc)
{
    //debug
    // SDL_Log(std::to_string(Random::IDistribution(0, 10)).c_str());

    gc.mScm->Update(gc);
    gc.mTurnm->Update();

    gc.mMapm->mSubMap->mCam->Move();

    gc.mUim->UpdateGridToolTip(gc.mUim->mToolTipGrid);
}

void SubMapState::HandleEvent(SDL_Event &e, GameContext& gc, float mouseX, float mouseY)
{
    gc.mUim->mDialogueUI->HandleEvent(e, gc, mouseX, mouseY);

    gc.mObjm->mEntm->HandleEvent(e, gc, gc.mMapm->mSubMap, mouseX, mouseY);
    gc.mObjm->mItm->HandleEvent(e, &gc, mouseX, mouseY);

    gc.mMapm->mSubMap->mCam->HandleEvent(e);
    gc.mMapm->mSubMap->HandleEvent(e, gc, mouseX, mouseY);

    gc.mSkm->HandleEvent(e, gc);

    gc.mUim->HandleUIEvent(e, gc, mouseX, mouseY);
    gc.mUim->HandleMapUIEvent(e, gc, gc.mMapm->mSubMap, mouseX, mouseY);
}

void SubMapState::Render(GameContext& gc)
{
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(System::sRenderer);
    
    SDL_SetRenderLogicalPresentation(System::sRenderer, 0, 0, SDL_LOGICAL_PRESENTATION_DISABLED);
    
    //맵 렌더링
    gc.mMapm->mSubMap->RenderOnUpdate();
    //아이템 렌더링
    gc.mObjm->mItm->Render();
    //엔티티 렌더링
    gc.mObjm->mEntm->RenderEntities(gc.mMapm->mSubMap);
    
    //ui들 렌더링
    gc.mUim->mFocusIcon->RenderByCam(gc.mMapm->mSubMap->mCam);
    gc.mUim->RenderMapUIs(gc.mMapm->mSubMap);
    gc.mUim->RenderUIs();

    gc.mUim->mDialogueUI->RenderOnUpdate();
    if (!gc.mUim->mDialogueUI->mIsRender) gc.mUim->mTurnOverBtn->Render();

    SDL_RenderPresent(System::sRenderer);
}

void CityViewState::Enter(GameContext& gc)
{
    SDL_Log("enter city view");
    Map* map = gc.mMapm->mCityMap;
    gc.mTurnm->Enter(map);
    gc.mUim->mToolTipGrid = map;
    gc.mMapm->mCurrentMap = map;

    //도시 맵 렌더링 플래그
    gc.mMapm->mCityMap->mIsMapUpdate = true;

    //사이드바
    gc.mUim->uiMap["titleButton"] = new Button(10, 10 + gc.mUim->mTopPanelH, 100, 50, "타이틀로", BtnType::Title);
    gc.mUim->uiMap["overMapButton"] = new Button(10, 70 + gc.mUim->mTopPanelH, 100, 50, "오버맵", BtnType::OverMap);

    //탑 바
    gc.mUim->InitTopBar();
    gc.mUim->InitUIs();

    gc.mUim->mTurnOverBtn->mType = BtnType::CityMapTurnOver;
}

void CityViewState::Exit(GameContext& gc)
{
    SDL_Log("exit city view");
    gc.mUim->DestroyUIs(); //ui파괴
}

void CityViewState::Update(GameContext& gc)
{
    gc.mScm->Update(gc);

    gc.mUim->UpdateGridToolTip(gc.mUim->mToolTipGrid);
}

void CityViewState::HandleEvent(SDL_Event &e, GameContext& gc, float mouseX, float mouseY)
{
    gc.mUim->HandleUIEvent(e, gc, mouseX, mouseY);

    gc.mUim->HandleMapUIEvent(e, gc, gc.mMapm->mCityMap, mouseX, mouseY);
}

void CityViewState::Render(GameContext& gc)
{
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(System::sRenderer);

    //도시 맵 렌더링
    gc.mMapm->mCityMap->RenderOnUpdate();
    //아이템 렌더링
    gc.mObjm->mItm->Render();
    //툴팁 렌더링
    gc.mUim->mToolTip->Render();
    gc.mUim->RenderUIs();    //ui렌더링
    gc.mUim->RenderMapUIs(gc.mMapm->mCityMap);
    //턴 종료 버튼
    if (!gc.mUim->mDialogueUI->mIsRender) gc.mUim->mTurnOverBtn->Render();

    gc.mRenderM->RenderFps();
    SDL_RenderPresent(System::sRenderer);
}

