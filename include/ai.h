#pragma once

#include <vector>

struct GameContext;
class AIState; class AIManager;
class Entity; class Npc;

using json = nlohmann::json;


class AI {
    public:
    AI(GameContext* gc);
    ~AI();
    void Destroy();
    
    AIState* Transition(GameContext* gctx, Npc* npc);

    void TakeTurn(Npc* npc);
    void UpdateSkillQueue(Npc* npc);

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
    virtual void UpdateSkillQueue(Npc* npc);

    GameContext* mGc {nullptr};
};

class CombatState : public AIState {
    public:
    CombatState(GameContext* gc);

    void UpdateSkillQueue(Npc* npc) override;
};

class IdleState : public AIState {
    public:
    IdleState(GameContext* gc);
    void Destroy() override;

    void UpdateSkillQueue(Npc* npc) override;
};

class FleeState : public AIState {
    public:
    FleeState(GameContext* gc);

    void UpdateSkillQueue(Npc* npc) override;
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
