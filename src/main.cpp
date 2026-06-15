#include "pch.h"

#include <SDL3/SDL.h> 

#include "ui.h"
#include "system.h"
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
    RenderManager rend;

    UIManager uim;
    uim.InitUIs();

    //메인 루프
    while (quit == false) {

        bool isUIUpdate = true; //ui 업데이트용 플래그 변수

        quit = sys.HandleEvents(e, uim);

        rend.RenderThings(uim);

        rend.AdjustFps(timer);
    }

    sys.Close();
}

