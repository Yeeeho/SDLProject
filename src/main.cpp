#include "pch.h"

#include <SDL3/SDL.h> 

#include "system.h"
#include "game_context.h"
#include "scenario.h"
#include "game_object.h"
#include "game_state.h"
#include "ui.h"
#include "render.h"

int main() {
    System sys;

    if (sys.Init() == false) {
        SDL_Log("Unable to init SDL");
    }
    if (sys.LoadMedia() == false) {
        SDL_Log("Unable to load media");
    }

    bool quit {false};

    SDL_Event e;
    SDL_zero(e);

    Timer timer;

    GameContext gc;

    //로딩
    sys.LoadData(*gc.mObjm); //json 데이터베이스 로드
    sys.LoadObjects(*gc.mObjm); //게임에서 사용할 객체 로드
    sys.LoadUIs(*gc.mUim, *gc.mObjm); //ui객체 로드

    //초기 게임 상태를 현재 상태에 저장한다.
    gc.mGsm->mCurrentState = gc.mGsm->mIs;
    gc.mGsm->mCurrentState->Enter(gc);

    //메인 루프
    while (quit == false) {

        quit = sys.HandleEvents(e, gc);

        gc.mGsm->mCurrentState->Update(gc);

        //게임 상태를 바꾼다.
        if (gc.mGsm->mIsStateChange) {
            gc.mGsm->SetCurrentState(gc);
        }

        gc.mGsm->mCurrentState->Render(gc);

        gc.mRenderM->AdjustFps(timer);
    }

    sys.Close();
}

