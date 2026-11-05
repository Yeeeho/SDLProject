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

    //로그 출력 옵션
    //로그 중요도
    inline static bool sLogInfoOn {false};
    inline static bool sLogVerboseOn {false};
    inline static bool sLogWarnOn {false};
    //로그 옵션
    inline static bool sLogOptionAIOn {false};
    inline static bool sLogOptionMathOn {false};

    //기본 색깔
    inline const static SDL_Color kTc {0x00, 0xB0, 0x00, 0xFF};
    inline const static SDL_Color kWh {0xF0, 0xF0, 0xF0, 0xFF};
    inline const static SDL_Color kY {0xB0, 0xB0, 0x00, 0xFF};

    inline static int sWindowWidth {1920};
    inline static int sWindowHeight {1080};

    //인벤토리 제한
    inline const static int sInvCap {120};

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
    inline static bool sIsActionPerformed {false}; //이벤트를 루프당 한번 실행시키기 위한 플래그
    inline static bool sIsLeftMouseClicked {false};

    /*함수 선언*/

    bool Init();

    //초기 설정 로드
    bool LoadConfig();

    //외부 데이터(json등) 로드
    bool LoadData(ObjectManager&);

    bool LoadMedia(); //폰트, 사운드 등 로드
    bool LoadObjects(GameContext& gc); //객체 로드
    bool LoadUIs(UIManager&, ObjectManager&); //ui 객체 로드

    void Close();
    bool HandleEvents(SDL_Event& e, GameContext& gc);

    private:
};

namespace Log {
    //정보 로그
    void LogInfo(std::string message, bool option);
    void LogVerbose(std::string message, bool option);
    void LogWarn(std::string message, bool option);
};

class Timer {
    public:
    Uint64 mNs = 0;
    Uint64 mMs = 0;

    Uint64 StoreProgramTick();
    Uint64 GetPreviousTick();

    Uint64 StoreProgramTickMs();
};

