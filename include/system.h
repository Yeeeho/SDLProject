#pragma once

#include <SDL3/SDL.h>

class UIManager;
class ObjectManager;
class GameState;

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

    /*함수 선언*/

    bool Init();
    bool LoadMedia();
    void Close();
    bool HandleEvents(SDL_Event& e, UIManager& uim, ObjectManager& objm, GameState* currentSt);

    private:
};

class Timer {
    public:
    Uint64 mNs = 0;

    Uint64 StoreProgramTick();
    Uint64 GetPreviousTick();
};

class Map;
class ObjectManager {
    public:
    ObjectManager();

    Map* map{nullptr};
};