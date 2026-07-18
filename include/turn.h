#pragma once

class Entity; class Map;

class TurnManager {
    public:
    TurnManager() = default;
    virtual ~TurnManager();

    virtual void Update();
    virtual void TakeTurn(Entity* ent);

    void UpdateEntityQueue();
    bool mIsQueueUpdate {true};
    void UpdateTurn(); //턴 업데이트
    bool mIsTurnUpdate {true};

    int mNpcIdx {0}; //맵에 있는 엔티티의 인덱스를 저장함.
    int mNpcMaxNum {0};
    int mPawnIdx {0};
    int mPawnMaxNum {0};

    Map* mCurrentMap {nullptr};
};

class OverMapTurnManager : public TurnManager {
    public:
    OverMapTurnManager();
    
    void IncOverMapTurn();
    int GetOverMapTurn();
    void SetOverMapTurn(int turn); //디버깅용

    void Update() override;
    void TakeTurn(Entity* ent) override;

    private:
    int mOverMapTurn {0};
    int mOverMapTurnCap {4};
};

class SubMapTurnManager : public TurnManager {
    public:
    void EnterMap(Map* map); //맵에 처음 들어가면 실행된다.

    void IncSubMapTurn();
    int GetSubMapTurn();
    void SetSubMapTurn(int turn);

    void Update() override;
    void TakeTurn(Entity* ent) override;

    int mSubMapTurn {0}; 
};

class CityMapTurnManager : public TurnManager {

};

struct TurnManagers {
    public:

    CityMapTurnManager mCmtm;
    SubMapTurnManager mSmtm;
    OverMapTurnManager mOmtm;
};