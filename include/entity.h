#pragma once

#include <string>
#include <unordered_map>

//전방 선언
class Entity;
class Pawn;
class ObjectManager;
class Texture;

//일반 팀
class Team {
    public:
    Team() = default;
    Team(std::string path, int id);

    //식별용 아이디
    int mId = 0;

    //오버맵 위치
    int mMapPosX, mMapPosY = 0;

    //실제 팀원인 엔티티 객체들을 저장하는 컨테이너
    std::vector<Entity*> mTeamMates; 

    //텍스처
    Texture* mTeamTex{nullptr};
};

//플레이어(부하) 팀
class PawnTeam : public Team{
    public:
    PawnTeam(std::string path, int id);

    int supply = 0; //팀이 보유한 보급품량
};

enum class TeamSetting {
    MaxTeam = 32, MaxPawnTeam = 4
};

class TeamManager {
    public:
    TeamManager();

    Team* mTeamTable[static_cast<int>(TeamSetting::MaxTeam)]; //비 플레이어 팀 정보를 저장하는 컨테이너
    PawnTeam* mPawnTeamTable[static_cast<int>(TeamSetting::MaxPawnTeam)]; //플레이어 팀 정보를 저장하는 컨테이너

    //임시 텍스처   
    SDL_Texture* mTempTex{nullptr};
};

class Item;
class Equipment;
enum class EqType;

enum class EntitySetting {
    MaxEnt = 64, MaxPawn = 16
};

class EntityManager {
    public:
    EntityManager(ObjectManager& objm);
    
    Entity* mEntTable[static_cast<int>(EntitySetting::MaxEnt)];
    Pawn* mPawnTable[static_cast<int>(EntitySetting::MaxPawn)];
};

class Entity {
    public:
    Entity() = default;
    Entity(std::string name, int id);

    std::string mName;
    
    int mId; //뭔지알지?

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
    int mSupplyDemand = 0; //턴당 요구 보급품량

    //텍스처
};

enum class PawnType {
    Null, Unique, Procedural
};

class Pawn : public Entity {
    public:
    Pawn(const ObjectManager& objm, std::string name, PawnType pType, int id);
};