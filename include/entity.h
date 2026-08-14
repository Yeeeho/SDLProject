#pragma once

#include <string>
#include <unordered_map>

//전방 선언
struct GameContext;
class Entity;
class Map;
class Pawn;
class UIManager;
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
    void RenderOnUpdate(Map* map);

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
class Skill;
enum class SkillCode;

enum class EntitySetting {
    MaxEnt = 64, MaxPawn = 16
};

class EntityManager {
    public:
    EntityManager(ObjectManager& objm);
    
    void KillEntityOnMap(GameContext& gc, Map* map, Entity* ent);

    //엔티티 할당 함수
    void AllocEntityOnTable(ObjectManager& objm, std::string name, int xMapPos, int yMapPos, int id);
    void AllocPawnOnTable(ObjectManager& objm, std::string name, PawnType pType, int id);
    //엔티티 할당 해제 함수
    void DeallocEntityOnTable(ObjectManager& objm, int id);
    void DeallocPawnOnTable(ObjectManager& objm, int id);

    //맵 상호작용
    void SpawnEntityOnMap(ObjectManager& objm, Map* map, Entity* ent);
    void SpawnEntityOnMap(ObjectManager& objm, Map* map, Entity* ent, int tileId);
    void DespawnEntity(ObjectManager& objm, Map* map, Entity* ent);
    
    //아이템 상호작용
    bool PickUpItem(GameContext& gc, int tileId, int itemId, Pawn* p);
    void DropItem();

    //업데이트
    void Update(ObjectManager& objm);
    //이벤트 핸들링
    void HandleEvent(SDL_Event& e, GameContext& gc, Map* map, float mouseX, float mouseY);
    void HandleEntityEvent(SDL_Event& e, GameContext& gc, Map* map, Entity* ent, float mx, float my);

    void FocusEntity(GameContext& gc, Map* map, Entity* ent); //엔티티를 포커스로 설정함.

    //렌더링
    void RenderEntities(Map* map);

    Uint64 mTick {0};
    Uint64 mMaxMs {500};
    
    Entity* mEntTable[static_cast<int>(EntitySetting::MaxEnt)];
    Pawn* mPawnTable[static_cast<int>(EntitySetting::MaxPawn)];

    Entity* mPrevFocusedEnt {nullptr};
    Entity* mFocusedEnt {nullptr};
};

enum class Demeanor {
    Neutral, Hostile, Friendly
};

struct BodyPart {
    public:
    int mCount {0};
    int mStatus {0}; //타입 바꿔야한다. 지금은 임시
};

class Entity {
    public:
    Entity() = default;
    Entity(std::string name, int id);

    //식별 정보
    std::string mName {""}; //이름
    Demeanor mDemeanor {Demeanor::Neutral}; //태도 (적대적, 중립..)

    int mId;
    bool mIsPawn {false};

    //TODO: 텍스처는 매니저만 관리하게 리팩토링 해야됨
    Texture* mTexture {nullptr}; //엔티티 텍스처

    //맵 관련 
    bool mIsOnMap {false}; //맵에 있나?
    int mTileId {-1}; //지금 서있는 타일 id
    int mMapX {-1}; //좌표
    int mMapY {-1};
    int mXspeed {0};
    int mYspeed {0};

    //부위 정보
    BodyPart mHead = {0, 0};
    BodyPart mTorso = {0, 0};
    BodyPart mLower = {0, 0};
    BodyPart mHand = {0, 0};
    BodyPart mFoot = {0, 0};

    //범용 스탯
    int mStr {0};
    int mEnd {0};
    int mDex {0};
    int mPer {0};
    int mAgi {0};
    int mWil {0};
    int mInt {0};
    int mSpd {0};

    //전투 스탯, 최대 스탯(체력 등)은 범용 스탯에 따라 결정되며, 뭐 헬퍼 함수 등으로 그때그때 구하게 한다.
    int mCurHp {0}; //현재 체력.
    int mCurSp {0}; //현재 지구력
    int mCurAp {0}; //현재 행동력
    
    float mWeight {0.f}; //엔티티 무게. kg단위 참고
    
    //패시브 플래그

    //알고 있는 기술들
    std::vector<Skill*> mSkills;

    std::unordered_map<EqType, Equipment*> mEqs; //실제로 장비한 장비들 컨테이너
    //여행용 스탯
    //턴당 요구 보급품량 << 헬퍼에서 연산
};

enum class PawnType {
    Null, Unique, Procedural
};

class Pawn : public Entity {
    public:
    Pawn(const ObjectManager& objm, std::string name, PawnType pType, int id);

    std::string mCustomName {""};

    PawnType mType;

    std::map<int, Item*> mInventory;

    std::vector<std::string> mQuickSkills;
};

//스탯 관련 연산을 보조해주는 헬퍼 클래스에용
class StatHelper {
    public:
    int GetMaxHp(Entity* ent); //최대체력을 계산한다.
    int GetMaxSp(Entity* ent); //최대 sp를 계산한다.
    int GetMaxAp(Entity* ent); //최대 ap를 계산한다.
    
    //무게 계산
    float GetTotalWeight(Entity* ent);
    int GetMediumWeightLimit(Entity* ent); //페널티가 없는 무게 상한을 계산한다.
    int GetMaxWeightLimit(Entity* ent); //최대 무게 상한을 계산한다.
    //이동력 계산
    int GetApPerTileMove(Entity* ent); //타일당 표준 이동력을 계산한다.
    //캐릭터 최종 방어력 계산
    int GetTotalArmor(Entity* ent);
    //캐릭터 회복 수치 계산
    int GetHpRegen(Entity* ent);
    int GetSpRegen(Entity* ent);
    int GetApRegen(Entity* ent);
};

//엔티티용 유틸리티 클래스
class EntityUtil {
    public:
    SDL_Color GetDemeanorColor(Entity* ent);
    SDL_Color GetRDemeanorColor(Entity* ent);
};
