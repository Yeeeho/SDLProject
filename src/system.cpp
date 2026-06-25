#include "pch.h"

#include "system.h"
#include "map.h"
#include "city.h"
#include "game_json.h"
#include "game_object.h"
#include "game_state.h"
#include "ui.h"

bool System::Init()
{
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == false) {
        SDL_Log("couldn't init video or audio");
        SDL_Log(SDL_GetError());
        success = false;
    }
    if (SDL_CreateWindowAndRenderer("MistEater", sWindowWidth, sWindowHeight, 0, &sWindow, &sRenderer) == false) {
        SDL_Log("couldn't create window and renderer");
        SDL_Log(SDL_GetError());
        success = false;
    }
    else {
        SDL_SetWindowBordered(sWindow, true);
        SDL_MaximizeWindow(sWindow);
        SDL_SetRenderLogicalPresentation(sRenderer, 1280, 720, SDL_LOGICAL_PRESENTATION_DISABLED);
    }

    //수직동기화
    if (SDL_SetRenderVSync(sRenderer, 1) == false) {
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

bool System::LoadData(ObjectManager& objm)
{
    bool success = true;

    objm.mJsm = new JsonManager();
    objm.mJsm->LoadJsonFile(objm.mJsm->mPawnDb, "data/entity/pawn.json");
    objm.mJsm->LoadJsonFile(objm.mJsm->mEntDb, "data/entity/entity.json");
    objm.mJsm->LoadJsonFile(objm.mJsm->mEquipmentDb, "data/item/equipment.json");

    return success;
}

bool System::LoadMedia()
{
    bool success = true;

    std::string fontPath{"fonts/MaruMinyaHangul.ttf"};
    sFont = TTF_OpenFont(fontPath.c_str(), 20);
    sFont40 = TTF_OpenFont(fontPath.c_str(), 40);
    if (sFont == nullptr) {
        SDL_Log("Could not load %s, SDL_ttf Error: %s\n", fontPath.c_str(), SDL_GetError());
        success = false;     
    }
    else {
        TTF_SetFontHinting(System::sFont, TTF_HINTING_NONE);
    }
    return success;
}

bool System::LoadObjects(ObjectManager& objm) //메인 루프 전에 한번만 호출함.
{
    bool success = true;

    objm.map = new Map(3, 3); //월드 맵 객체 생성
    objm.map->GenerateMapTiles();
    objm.map->mMapTiles[5]->ChangeTexture("images/map/city.png");

    objm.cityMap = new CityMap(); //도시 맵 객체 생성
    objm.cityMap->GenerateFacs(6, 5, 100); //포함된 시설 객체들 생성

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

bool System::HandleEvents(SDL_Event& e, UIManager& uim, ObjectManager& objm, GameStateManager& gsm)
{
    bool quit = false;

    //마우스 위치 계산
    float mouseX, mouseY = -1.f;
    SDL_GetMouseState(&mouseX, &mouseY);

    //이벤트 큐에 이벤트가 있을때
    while (SDL_PollEvent(&e) == true) {
        if (e.type == SDL_EVENT_QUIT) quit = true;

        //현재 상태에 있는 매니저들의 이벤트 핸들링
        gsm.mCurrentState->HandleEvent(e, uim, objm, gsm, mouseX, mouseY);
    }

    return quit;
}

Uint64 Timer::StoreProgramTick()
{
    Uint64 ns = SDL_GetTicksNS();
    mNs = ns;
    return ns;
}

Uint64 Timer::GetPreviousTick()
{
    return mNs;
}
