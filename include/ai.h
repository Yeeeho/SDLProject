#pragma once

#include <vector>

struct GameContext;
class AIState; class AIManager;
class Entity;

using json = nlohmann::json;


class AI {
    public:
    AI(GameContext* gc);
    ~AI();
    void Destroy();
    
    AIState* Transition(GameContext* gctx, Entity* ent);

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

    void Attack(Entity* ent);
    void Defense(Entity* ent);
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

class AIManager {
    public:
    AIManager();
    json mAIDb;
};

namespace AIHelper {
    json GetAIData(GameContext* gctx, Entity* ent);

    int GetThreatValue(Entity* ent);
};
