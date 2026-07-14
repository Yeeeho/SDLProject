#pragma once

#include <string>
#include <unordered_map>

//전방 선언
class Entity;
class Map;
class Pawn;
class ObjectManager;
class Texture;

enum class TeamType {
    Unknown, Friendly, Enemy, Neutral
};

//일반 팀
class Team {
    public:
    Team() = default;
    Team(std::string path, int id);

    TeamType mType {TeamType::Neutral};
    //식별용 아이디
    int mId = 0;
    //게임에서 사용할 이름
    std::string mName = "";

    //오버맵에 존재하는가, 렌더링 제어용 플래그
    bool isOnMap = false;
    //오버맵 위치
    int mMapPosX, mMapPosY = 0;
    int mTileId;

    //팀 멤버 제한
    int mMaxMember = 4;
    //실제 팀원인 엔티티 객체들을 저장하는 컨테이너
    std::unordered_map<int ,Entity*> mTeamMates; 

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

class MapTile;

class TeamManager {
    public:
    TeamManager();

    //렌더링 관련 함수
    void RenderOnUpdate();

    //테이블에 실제 데이터를 할당하고 해제하는 함수
    void AllocTeamOnTable(std::string name, std::string path, int id);
    void DeallocTeamOnTable(int id);
    //우리 팀 할당/해제
    void AllocPTeamOnTable(std::string name, int id);
    void DeallocPTeamOnTable(int id);

    //맵 관련
    //생성
    void SpawnTeamOnMap(Map* map, Team* team, int id);
    void SpawnTeamOnMap(Map* map, Team* team, int x, int y);
    void LoadDataInTile(MapTile* tile, Team* team); //타일의 참조 데이터 컨테이너에 팀 데이터를 삽입한다.

    //팀에 엔티티 넣고 빼는 함수
    void PutEntInTeam(Team* team, Entity* ent); //팀에 엔티티 삽입
    void OutEntInTeam(Team* team, int id); //아이디에 해당하는 엔티티 제외

    //업데이트 여부
    bool mIsTeamUpdate = false;

    //데이터 테이블
    Team* mTeamTable[static_cast<int>(TeamSetting::MaxTeam)]; //비 플레이어 팀 정보를 저장하는 컨테이너
    PawnTeam* mPawnTeamTable[static_cast<int>(TeamSetting::MaxPawnTeam)]; //플레이어 팀 정보를 저장하는 컨테이너

    //임시 텍스처   
    SDL_Texture* mTempTex{nullptr};
};

class Item;
class Equipment;
enum class PawnType;
enum class EqType;

enum class EntitySetting {
    MaxEnt = 64, MaxPawn = 16
};

class EntityManager {
    public:
    EntityManager(ObjectManager& objm);
    
    //엔티티 할당 함수
    void AllocEntityOnTable(ObjectManager& objm, std::string name, int id);
    void AllocPawnOnTable(ObjectManager& objm, std::string name, PawnType pType, int id);
    //엔티티 할당 해제 함수
    void DeallocEntityOnTable(ObjectManager& objm, int id);
    void DeallocPawnOnTable(ObjectManager& objm, int id);

    Entity* mEntTable[static_cast<int>(EntitySetting::MaxEnt)];
    Pawn* mPawnTable[static_cast<int>(EntitySetting::MaxPawn)];
};

class Entity {
    public:
    Entity() = default;
    Entity(std::string name, int id);

    std::string mName;
    std::string mRace;

    int mId; //식별용 아이디
    
    Texture* mTexture {nullptr}; //엔티티 텍스처

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

    PawnType mType;
};