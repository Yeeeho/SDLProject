#include "pch.h"

#include "game_state.h"
#include "map.h"
#include "system.h"
#include "render.h"
#include "square.h"
#include "ui.h"

void IntroState::Enter(UIManager& uim, ObjectManager& objm)
{
    SDL_Log("enter intro");
    uim.uiMap["introUI"] = new Button(new Square(10, 10, 300, 100), "버튼을 누르면 시작한다잉.", BtnType::Start);
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
    rend.RenderThings(uim, objm);
}

void OverMapState::Enter(UIManager& uim, ObjectManager& objm)
{
    SDL_Log("enter overmap");
    objm.map = new Map(3, 3); //맵 생성
}

void OverMapState::Exit(UIManager& uim, ObjectManager& objm)
{
    SDL_Log("exit overmap");

    uim.DestroyUIs(); //ui파괴

    delete objm.map;
    objm.map = nullptr; //해제해준다.
}

void OverMapState::Update(UIManager& uim, ObjectManager& objm, GameStateManager& gsm)
{
}

void OverMapState::HandleEvent(SDL_Event& e, UIManager& uim, ObjectManager& objm, GameStateManager& gsm, float mouseX, float mouseY)
{
}

void OverMapState::Render(RenderManager& rend, UIManager& uim, ObjectManager& objm)
{
    rend.RenderThings(uim, objm);
}

GameStateManager::GameStateManager()
{
    mIs = new IntroState();
    mOms = new OverMapState();
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
