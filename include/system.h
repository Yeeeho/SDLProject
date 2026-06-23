#pragma once

#include <SDL3/SDL.h>

class UIManager;
class ObjectManager;
class GameState;
class GameStateManager;

class System {
    public:

    inline static int sWindowWidth{1280};
    inline static int sWindowHeight{720};

    //윈도우
    inline static SDL_Window* sWindow{nullptr};
    //렌더러
    inline static SDL_Renderer* sRenderer{nullptr};
    //믹서
    inline static MIX_Mixer* sMixer{nullptr};
    //폰트
    inline static TTF_Font* sFont{nullptr};
    inline static TTF_Font* sFont40{nullptr};

    /*함수 선언*/

    bool Init();
    bool LoadMedia();
    bool LoadObjects(ObjectManager&);
    void Close();
    bool HandleEvents(SDL_Event& e, UIManager& uim, ObjectManager& objm, GameStateManager& gsm);

    private:
};

class Timer {
    public:
    Uint64 mNs = 0;

    Uint64 StoreProgramTick();
    Uint64 GetPreviousTick();
};

//전방 선언
class Map;
class CityMap;
//아직은 멤버 변수 삭제 로직은 안만듬.
class ObjectManager {
    public:
    ObjectManager();

    void RenderObjects();
    void DestroyObjects();

    //오버맵에서 사용할 월드맵 객체다.
    Map* map{nullptr};
    //도시 뷰에서 사용할 도시 맵 객체다.
    CityMap* cityMap{nullptr};
};