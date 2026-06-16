#include "pch.h"

#include "game_state.h"
#include "system.h"
#include "render.h"
#include "square.h"
#include "ui.h"

void IntroState::Enter(UIManager& uim, ObjectManager& objm)
{
    SDL_Log("enter intro");
    uim.uiMap["introUI"] = new Button(new Square(10, 10, 100, 100), "버튼을 누르면 시작한다잉.");
}

void IntroState::Exit(UIManager& uim, ObjectManager& objm)
{
    SDL_Log("exit intro");
}

void IntroState::Update(UIManager& uim, ObjectManager& objm)
{

}

void IntroState::HandleEvent(SDL_Event& e,UIManager& uim, ObjectManager& objm, float mouseX, float mouseY)
{
    for (auto ui : uim.uiMap) {
        ui.second->HandleEvent(e, mouseX, mouseY);
    }
}

void IntroState::Render(RenderManager& rend, UIManager& uim, ObjectManager& objm)
{
}

void OverMapState::Enter(UIManager& uim, ObjectManager& objm)
{
    SDL_Log("enter overmap");
}

void OverMapState::Exit(UIManager& uim, ObjectManager& objm)
{
    SDL_Log("exit overmap");
}

void OverMapState::Update(UIManager& uim, ObjectManager& objm)
{
}

void OverMapState::HandleEvent(SDL_Event& e, UIManager& uim, ObjectManager& objm, float mouseX, float mouseY)
{
}

void OverMapState::Render(RenderManager& rend, UIManager& uim, ObjectManager& objm)
{
}

GameStateManager::GameStateManager()
{
    mIs = new IntroState();
    mOms = new OverMapState();
}

GameState *GameStateManager::SetCurrentState(GameState *currentState)
{
    //상태변화 플래그가 거짓이면 현재 상태를 리턴
    if (!currentState->mIsStateChange) return currentState;
    //거짓이 아니면
    currentState->mIsStateChange = false; //플래그 초기화

    //다음상태가 널인지를 확인.
    if (currentState->mNextState != nullptr) {
        //다음 상태를 임시 반환 객체에 할당해줌
        GameState* retState = currentState->mNextState;
        currentState->mNextState = nullptr; //초기화
        return retState;
    }
    else {
        //다음상태가 널이면 그냥 현재 상태를 반환함.
        return currentState;
    }
}
