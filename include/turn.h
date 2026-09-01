#pragma once

class Entity; class Npc;
class Map;
struct GameContext;

class TurnManager {
    public:
    TurnManager(GameContext* gc);

    void Enter(Map* map);

    void HandleEvent(SDL_Event& e, float mx, float my);

    void Update();

    //턴 제어 관련
    void IncTurn();
    void IncCurMapTurn();
    int GetCurMapTurn();
    void IncGlobalTurn();
    int GetGlobalTurn();

    void TakeTurn(Entity* ent); //지금 턴인 엔티티 타겟

    void ClearTargets();
    void UpdateEntityQueue();
    bool mIsQueueUpdate {true};

    void UpdateSkillQueue(Npc* npc); //엔피시 스킬 큐 업데이트
    void UpdateTurn(); //턴 업데이트

    GameContext* mGc {nullptr};

    int mNpcIdx {0}; //맵에 있는 엔티티의 인덱스를 저장함.
    int mPawnIdx {0};
    
    Entity* mCurrentTarget {nullptr};
    Entity* mPrevTarget {nullptr}; //이전 객체 캐싱
    Map* mCurrentMap {nullptr};

    private:
    int mSubMapTurn {0};
    int mGlobalTurn {0};
};
