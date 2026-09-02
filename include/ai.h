#pragma once

#include <vector>

struct GameContext;
class AIState; class AIManager;
class CombatState; class IdleState; class FleeState;
class Entity; class Npc;
class Map;

using json = nlohmann::json;


class AI {
    public:
    AI(GameContext* gc);
    ~AI();
    void Destroy();
    
    void Transition(GameContext* gctx, Npc* npc);

    void TakeTurn(GameContext* gctx, Npc* npc);
    void UpdateSkillQueue(GameContext* gctx, Npc* npc);

    GameContext* mGc {nullptr};

    IdleState* mIdleState {nullptr};
    CombatState* mCombatState {nullptr};
    FleeState* mFleeState {nullptr};

    AIState* mCurrentState {nullptr};
};


class AIState {
    public:
    AIState() = default;
    ~AIState() = default;
    virtual void Destroy();

    virtual void Enter();
    virtual void Exit();
    virtual void UpdateSkillQueue(Npc* npc);

    void NavigateQueue(Npc* npc, Map* map, int tileId);

    GameContext* mGc {nullptr};
    int mPrevTileId {-1};
};

class CombatState : public AIState {
    public:
    CombatState(GameContext* gc);
    ~CombatState();
    void Destroy() override;

    void UpdateSkillQueue(Npc* npc) override;
};

class IdleState : public AIState {
    public:
    IdleState(GameContext* gc);
    ~IdleState();
    void Destroy() override;

    void UpdateSkillQueue(Npc* npc) override;
};

class FleeState : public AIState {
    public:
    FleeState(GameContext* gc);
    ~FleeState();
    void Destroy() override;

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
