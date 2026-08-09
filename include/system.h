#pragma once

#include <SDL3/SDL.h>
#include <json.hpp>

class UIManager;
class ObjectManager;
class GameState;
struct GameContext;
class GameStateManager;

using json = nlohmann::json;

class System {
    public:

    inline const static SDL_Color sTc {0x00, 0xB0, 0x00, 0xFF};

    inline static int sWindowWidth {1920};
    inline static int sWindowHeight {1080};

    //아이템 관련
    const static int sMaxItemId {1024};

    //스킬 관련 
    inline static int sQuickSlotCap {8};
    inline static int sSkillMemCap {16};

    //윈도우
    inline static SDL_Window* sWindow {nullptr};
    //렌더러
    inline static SDL_Renderer* sRenderer {nullptr};
    //믹서
    inline static MIX_Mixer* sMixer{nullptr};
    //폰트
    inline static TTF_Font* sFont {nullptr};
    inline static TTF_Font* sFont10 {nullptr};
    inline static TTF_Font* sFont40 {nullptr};
    //이벤트 핸들링 관련
    inline static bool sIsLeftMouseClicked {false};

    /*함수 선언*/

    bool Init();

    //외부 데이터(json등) 로드
    bool LoadData(ObjectManager&);

    bool LoadMedia(); //폰트, 사운드 등 로드
    bool LoadObjects(ObjectManager&); //객체 로드
    bool LoadUIs(UIManager&, ObjectManager&); //ui 객체 로드

    void Close();
    bool HandleEvents(SDL_Event& e, GameContext& gc);

    private:
};

class Timer {
    public:
    Uint64 mNs = 0;
    Uint64 mMs = 0;

    Uint64 StoreProgramTick();
    Uint64 GetPreviousTick();

    Uint64 StoreProgramTickMs();
};

