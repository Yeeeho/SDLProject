#pragma once

#include <SDL3/SDL.h>

class System {
    public:

    static constexpr int kWindowWidth{800};
    static constexpr int kWindowHeight{600};

    //윈도우
    inline static SDL_Window* kWindow{nullptr};
    //렌더러
    inline static SDL_Renderer* kRenderer{nullptr};
    //믹서
    inline static MIX_Mixer* kMixer{nullptr};
    //폰트
    inline static TTF_Font* kFont{nullptr};

    /*함수 선언*/
    //초기화 함수
    bool Init();
    bool LoadMedia();
    void Close();
    private:

};