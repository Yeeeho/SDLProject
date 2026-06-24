#pragma once

//무거운 객체가 되지 않게 최대한 포인터를 이용한다.

//전방 선언
class Map;
class CityMap;
class JsonManager;
//클래스 선언
class ObjectManager {
    public:
    ObjectManager();

    void RenderObjects();
    void DestroyObjects();

    //json 매니저
    JsonManager* mJsm{nullptr};

    //오버맵에서 사용할 월드맵 객체다.
    Map* map{nullptr};
    //도시 뷰에서 사용할 도시 맵 객체다.
    CityMap* cityMap{nullptr};
};
