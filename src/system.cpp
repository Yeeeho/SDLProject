#include "pch.h"

#include "system.h"

bool System::Init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == false) {
        SDL_Log("couldn't init video or audio");
        SDL_Log(SDL_GetError());
        success = false;
    }
    if (SDL_CreateWindowAndRenderer("MistDigger", kWindowWidth, kWindowHeight, 0, &sWindow, &sRenderer) == false) {
        SDL_Log("couldn't create window and renderer");
        SDL_Log(SDL_GetError());
        success = false;
    }
    //수직동기화
    if (SDL_SetRenderVSync(sRenderer, 1) == false) {
        SDL_Log(SDL_GetError());
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
    sMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (sMixer == nullptr) {
        SDL_Log(SDL_GetError());
        success = false;        
    }

    return success;
}

bool System::LoadMedia()
{
    bool success = true;

    std::string fontPath{"fonts/FreeSans.ttf"};
    sFont = TTF_OpenFont(fontPath.c_str(), 28);
    if (sFont == nullptr) {
        SDL_Log("Could not load %s, SDL_ttf Error: %s\n", fontPath.c_str(), SDL_GetError());
        success = false;     
    }

    return success;
}

void System::Close()
{
    //믹서를 해제
    MIX_DestroyMixer(sMixer);

    //윈도우, 렌더러 해제
    SDL_DestroyWindow(sWindow);
    SDL_DestroyRenderer(sRenderer);

    //서브시스템 종료
    SDL_Quit();
    TTF_Quit();
    MIX_Quit();
}
