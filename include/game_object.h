#pragma once

//무거운 객체가 되지 않게 최대한 포인터를 이용한다.

//전방 선언
struct GameContext;
class Map;
class City;
class CityMap;
class ItemManager;
class JsonManager;
class TeamManager;
class EntityManager;
class SkillManager;
struct SkillContext;

//클래스 선언
class ObjectManager {
    public:
    ObjectManager(GameContext* gc);

    void DestroyObjects();

    //업데이트
    void UpdateObjects();
    void UpdateEntities(GameContext* gc);
    void UpdateSkCtx();
    
    bool mIsUpdating {false};
    bool mUpdateInQueue {false};

    SkillContext* mCurrentSkCtx {nullptr}; //현재 업데이트 대상인 스킬 컨텍스트
    std::queue<SkillContext*> mSkCtxUpdateQueue; //스킬컨텍스트 큐 업데이트용

    //참고용
    GameContext* mGc {nullptr};
    
    //아이템 매니저
    ItemManager* mItm {nullptr};
    //json 매니저
    JsonManager* mJsm{nullptr};
    //팀 매니저
    TeamManager* mTeamm{nullptr};
    //엔티티 매니저
    EntityManager* mEntm{nullptr};
    //도시 객체
    City* mCity {nullptr};

};
