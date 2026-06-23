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

    //게임 상태 매니저
    GameStateManager gsm;
    
    //게임에 쓸것들 미리 만들어놓기
    sys.LoadObjects(objm);
    
    //초기 게임 상태를 현재 상태에 저장한다.
    gsm.mCurrentState = gsm.mIs;
    gsm.mCurrentState->Enter(uim, objm);


    //메인 루프
    while (quit == false) {

        quit = sys.HandleEvents(e, uim, objm, gsm);

        gsm.mCurrentState->Update(uim, objm, gsm);

        //게임 상태를 바꾼다.
        if (gsm.mIsStateChange) {
            gsm.SetCurrentState(uim, objm);
        }

        gsm.mCurrentState->Render(rend, uim, objm);

        rend.AdjustFps(timer);
    }

    sys.Close();
}

