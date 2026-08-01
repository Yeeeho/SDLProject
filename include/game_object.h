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
