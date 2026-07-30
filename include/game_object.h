#pragma once

//무거운 객체가 되지 않게 최대한 포인터를 이용한다.

//전방 선언
class Map;
class City;
class CityMap;
class ItemManager;
class JsonManager;
class TeamManager;
class EntityManager;
class SkillManager;

//클래스 선언
class ObjectManager {
    public:
    ObjectManager();

    void RenderObjects();
    void DestroyObjects();

    //게임 시작용 오브젝트들 로드
    void InitStartObjects();

    //아이템 매니저
    ItemManager* mItm {nullptr};
    //json 매니저
    JsonManager* mJsm{nullptr};
    //팀 매니저
    TeamManager* mTeamm{nullptr};
    //엔티티 매니저
    EntityManager* mEntm{nullptr};
    //스킬 매니저
    SkillManager* mSkm{nullptr};

    //도시 객체
    City* mCity {nullptr};

    //오버맵에서 사용할 월드맵 객체다.
    Map* mMap {nullptr};
    //탐험/전투가 일어나는 맵이다.
    Map* mSubMap {nullptr};

};
