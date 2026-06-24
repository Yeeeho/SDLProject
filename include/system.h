#pragma once

#include <SDL3/SDL.h>
#include <json.hpp>

class UIManager;
class ObjectManager;
class GameState;
class GameStateManager;

using json = nlohmann::json;

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

    //외부 데이터(json등) 로드
    bool LoadData(ObjectManager&);

    //폰트 등을 로드
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

