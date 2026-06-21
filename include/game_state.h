#pragma once

#include <SDL3/SDL.h>

class RenderManager;
class UIManager;
class ObjectManager;
class GameStateManager;

class GameState {
    public:

    //이 밑으로는 다 가상함수다.
    //상태에 진입하면 한번 호출하는 녀석.
    virtual void Enter(UIManager& uim, ObjectManager& objm) = 0;
    //상태에서 벗어나면 한번 호출하는 친구.
    virtual void Exit(UIManager& uim, ObjectManager& objm) = 0;

    //핵심 로직이 들어있는 녀석. 상태 업데이트는 직접 하지 않고 상태 변환 플래그랑 다음 타겟 상태만 바꿔준다.
    virtual void Update(UIManager& uim, ObjectManager& objm, GameStateManager& gsm) = 0;
    //이벤트 핸들러. 상태 변환 플래그와 타겟 상태도 정해준다.
    virtual void HandleEvent(SDL_Event& e, UIManager& uim, ObjectManager& objm, GameStateManager& gsm, float mouseX, float mouseY) = 0;
    //ui와 오브젝트들을 렌더링하는 친구.
    virtual void Render(RenderManager& rend, UIManager& uim, ObjectManager& objm) = 0;

    virtual ~GameState() = default;
};

class IntroState : public GameState {
    public:

    void Enter(UIManager& uim, ObjectManager& objm) override;
    void Exit(UIManager& uim, ObjectManager& objm) override;

    void Update(UIManager& uim, ObjectManager& objm, GameStateManager& gsm) override;
    void HandleEvent(SDL_Event& e, UIManager& uim, ObjectManager& objm, GameStateManager& gsm, float mouseX, float mouseY) override;
    void Render(RenderManager& rend ,UIManager& uim, ObjectManager& objm) override;

    IntroState() = default;
    private:
};

class OverMapState : public GameState {
    public:

    void Enter(UIManager& uim, ObjectManager& objm) override;
    void Exit(UIManager& uim, ObjectManager& objm) override;

    void Update(UIManager& uim, ObjectManager& objm, GameStateManager& gsm) override;
    void HandleEvent(SDL_Event& e, UIManager& uim, ObjectManager& objm, GameStateManager& gsm, float mouseX, float mouseY) override;
    void Render(RenderManager& rend ,UIManager& uim, ObjectManager& objm) override;

    OverMapState() = default;
    private:
};

class CityViewState : public GameState {
    public:
    void Enter(UIManager& uim, ObjectManager& objm) override;
    void Exit(UIManager& uim, ObjectManager& objm) override;

    void Update(UIManager& uim, ObjectManager& objm, GameStateManager& gsm) override;
    void HandleEvent(SDL_Event& e, UIManager& uim, ObjectManager& objm, GameStateManager& gsm, float mouseX, float mouseY) override;
    void Render(RenderManager& rend ,UIManager& uim, ObjectManager& objm) override;

    CityViewState() = default;
};

//게임상태 매니저, 참조용 상태 객체들, 임시 상태 객체들, 상태를 바꾸는 메서드를 갖고있다.
class GameStateManager {
    public:
    //한번만 생성해서 한개만 사용해라.
    GameStateManager();

    //상태 변화 플래그
    bool mIsStateChange{false};
    //대기중인 다음 상태를 현재 상태로 바꿔주는 메서드.
    void SetCurrentState(UIManager& uim, ObjectManager& objm);

    //임시 상태들
    GameState* mCurrentState{nullptr}; //현재 상태
    GameState* mNextState{nullptr}; //다음에 바뀔 타겟 상태.

    //참조용 상태들. 해제하면 게임 터진다.
    IntroState* mIs{nullptr};
    OverMapState* mOms{nullptr};
    CityViewState* mCvs{nullptr};
};
