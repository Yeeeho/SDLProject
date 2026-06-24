#pragma once

#include <string>
#include <unordered_map>

//전방 선언
class Entity;
class ObjectManager;
class Texture;

class Team {
    public:
    Team(std::string path);
    
    int supply = 0;

    //실제 팀원인 엔티티 객체들을 저장하는 컨테이너
    Entity* mTeamMates{nullptr}; 

    Texture* mTeamTex{nullptr}; //텍스처
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
    
    std::unordered_map<EqType, Equipment*> mEqs; //실제로 장비한 장비들 컨테이너

    //여행용 스탯
    int mSupplyDemand; //턴당 요구 보급품량

    //텍스처
};

enum class PawnType {
    Unique, Procedural
};

class Pawn : public Entity {
    public:
    Pawn(const ObjectManager& objm, std::string name, PawnType pType);
};

class Enemy : public Entity {
    public:
    Enemy(const ObjectManager& objm, std::string name, PawnType pType);
};

