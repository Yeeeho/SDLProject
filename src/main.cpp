#include "pch.h"

#include <SDL3/SDL.h> 

#include "system.h"
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

    bool quit{false};

    SDL_Event e;
    SDL_zero(e);

    Timer timer;

    /*매니저 객체들을 스택 소환해준다*/
    RenderManager rend;
    UIManager uim;
    ObjectManager objm;

    //이새끼는 게임 상태들을 데리고 다닌다.
    GameStateManager gsm;

    //초기 게임 상태를 현재 상태에 저장한다.
    GameState* currentState = gsm.mIs;
    currentState->Enter(uim, objm);

    //메인 루프
    while (quit == false) {

        quit = sys.HandleEvents(e, uim, objm, currentState);

        currentState->Update(uim, objm);

        //게임 상태를 바꾼다.
        if (currentState->mIsStateChange) {
            currentState = gsm.SetCurrentState(currentState);
        }

        rend.RenderThings(uim);
        currentState->Render(rend, uim, objm);

        rend.AdjustFps(timer);
    }

    sys.Close();
}

