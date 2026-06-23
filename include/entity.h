#pragma once

#include <string>
#include <unordered_map>

class Entity;
class Team {
    public:
    Team();
    
    int supply = 0;
    
    Entity* teammates{nullptr};
};

class Item;
class Equipment;
enum class EqType;

class Entity {
    public:
    Entity() = default;
    Entity(std::string name);

    std::string mName;

    //전투용 스탯
    int mMaxHp; //max~ 는 최대치. 종족값
    int mCurHp; //cur~ 는 현재 상태.

    int mMaxAp;
    int mCurAp;

    int mMaxSpd;
    int mCurSpd;

    int mMaxAtk;
    int mCurAtk;

    int mMaxArmor;
    int mCurArmor;
    
    //여행용 스탯
    int mSupplyDemand; //턴당 요구 보급품량

    std::unordered_map<EqType, Equipment*> mEqs;
};


class Pawn : public Entity {
    public:
    Pawn(std::string name);
};

class Enemy : public Entity {
    public:
    Enemy(std::string name);
};