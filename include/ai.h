#pragma once

#include <vector>

struct GameContext;
class AIState;
class Entity;

class AI {
    public:
    AI(GameContext* gc);
    ~AI();
    void Destroy();
    
    void TakeTurn(Entity* ent);
    
    AIState* mCurrentState;

    GameContext* mGc {nullptr};
};


class AIState {
    public:
    AIState() = default;
    ~AIState() = default;
    virtual void Destroy();

    virtual void Enter();
    virtual void Exit();
    virtual void TakeTurn(Entity* ent);

    GameContext* mGc {nullptr};
};

class CombatState : public AIState {
    public:
    CombatState(GameContext* gc);
    void TakeTurn(Entity* ent) override;
};

class IdleState : public AIState {
    public:
    IdleState(GameContext* gc);
    void Destroy() override;

    void TakeTurn(Entity* ent) override;
};

class FleeState : public AIState {
    public:
    FleeState(GameContext* gc);
    void TakeTurn(Entity* ent) override;
};

class AIHelper {
    public:
    static int GetThreatValue(Entity* ent);
};