#pragma once

class Entity; class Map;

class TurnManager {
    public:
    TurnManager() = default;
    virtual ~TurnManager();

    //업데이트
    virtual void Update();

    //턴 제어 관련
    virtual void IncTurn();
    virtual int GetTurn();
    virtual void SetTurn(int turn); //디버깅용

    virtual void TakeTurn(Entity* ent); //지금 턴인 엔티티 타겟

    void UpdateEntityQueue();
    bool mIsQueueUpdate {true};
    void UpdateTurn(); //턴 업데이트
    bool mIsTurnUpdate {true};

    int mNpcIdx {0}; //맵에 있는 엔티티의 인덱스를 저장함.
    int mNpcMaxNum {0};
    int mPawnIdx {0};
    int mPawnMaxNum {0};

    int mCurrentTurn {0};

    Entity* mCurrentTarget {nullptr};
    Entity* mPrevTarget {nullptr}; //이전 객체 캐싱
    Map* mCurrentMap {nullptr};
};

class OverMapTurnManager : public TurnManager {
    public:
    OverMapTurnManager();
    
    void IncTurn() override;

    void Update() override;
    void TakeTurn(Entity* ent) override;

    int mTurnCap {0};
};

class SubMapTurnManager : public TurnManager {
    public:
    void EnterMap(Map* map); //맵에 처음 들어가면 실행된다.

    void Update() override;
    void TakeTurn(Entity* ent) override;
};

class CityMapTurnManager : public TurnManager {

};

struct TurnManagers {
    public:

    CityMapTurnManager mCmtm;
    SubMapTurnManager mSmtm;
    OverMapTurnManager mOmtm;
};