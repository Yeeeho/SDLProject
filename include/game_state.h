#pragma once

#include <SDL3/SDL.h>

class RenderManager;
class UIManager;
class ObjectManager;

class GameState {
    public:
    bool mIsStateChange = false;
    GameState* mNextState{nullptr};

    //이 밑으로는 다 가상함수다.
    virtual void Enter(UIManager& uim, ObjectManager& objm) = 0;
    virtual void Exit(UIManager& uim, ObjectManager& objm) = 0;

    virtual void Update(UIManager& uim, ObjectManager& objm) = 0;
    virtual void HandleEvent(SDL_Event& e, UIManager& uim, ObjectManager& objm, float mouseX, float mouseY) = 0;
    virtual void Render(RenderManager& rend, UIManager& uim, ObjectManager& objm) = 0;

    virtual ~GameState() = default;
};

class IntroState : public GameState {
    public:

    void Enter(UIManager& uim, ObjectManager& objm) override;
    void Exit(UIManager& uim, ObjectManager& objm) override;

    void Update(UIManager& uim, ObjectManager& objm) override;
    void HandleEvent(SDL_Event& e, UIManager& uim, ObjectManager& objm, float mouseX, float mouseY) override;
    void Render(RenderManager& rend ,UIManager& uim, ObjectManager& objm) override;

    IntroState() = default;
    private:
};

class OverMapState : public GameState {
    public:

    void Enter(UIManager& uim, ObjectManager& objm) override;
    void Exit(UIManager& uim, ObjectManager& objm) override;

    void Update(UIManager& uim, ObjectManager& objm) override;
    void HandleEvent(SDL_Event& e, UIManager& uim, ObjectManager& objm, float mouseX, float mouseY) override;
    void Render(RenderManager& rend ,UIManager& uim, ObjectManager& objm) override;

    OverMapState() = default;
    private:
};

//전방선언
class RenderManager;
class UIManager;
//게임상태 매니저
class GameStateManager {
    public:
    //한번만 생성해서 한개만 사용해라.
    GameStateManager();
    //상태 바꾸는 메서드
    GameState* SetCurrentState(GameState* currentState);

    IntroState *mIs{nullptr};
    OverMapState *mOms{nullptr};
};
