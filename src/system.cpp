#include "pch.h"

#include "system.h"

bool System::Init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == false) {
        SDL_Log(SDL_GetError());
        success = false;
    }
    if (SDL_CreateWindowAndRenderer("MistDigger", kWindowWidth, kWindowHeight, 0, &kWindow, &kRenderer) == false) {
        SDL_Log(SDL_GetError());
        success = false;
    }
    //수직동기화
    if (SDL_SetRenderVSync(kRenderer, 1)) {
        SDL_Log(SDL_GetError());
        success = false;        
    }
    //폰트 초기화
    if (TTF_Init() == false) {
        SDL_Log(SDL_GetError());
        success = false;        
    }
    //믹서 초기화
    if (MIX_Init() == false) {
        SDL_Log(SDL_GetError());
        success = false;        
    }
    //믹서 할당
    kMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (kMixer == nullptr) {
        SDL_Log(SDL_GetError());
        success = false;        
    }

    return success;
}

bool System::LoadMedia()
{
    bool success = true;

    std::string fontPath{"fonts/FreeSans.ttf"};

    return success;
}

void System::Close()
{
    //믹서를 해제
    MIX_DestroyMixer(kMixer);

    //윈도우, 렌더러 해제
    SDL_DestroyWindow(kWindow);
    SDL_DestroyRenderer(kRenderer);

    //서브시스템 종료
    SDL_Quit();
    TTF_Quit();
    MIX_Quit();
}
