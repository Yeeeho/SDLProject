#include "pch.h"

#include <SDL3/SDL.h> 

#include "system.h"
#include "square.h"
#include "texture.h"

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

    while (quit == false) {
        //이벤트 큐에 이벤트가 있을때
        while (SDL_PollEvent(&e) == true) {
            if (e.type == SDL_EVENT_QUIT) quit = true;
        }

        SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(System::sRenderer);

        Square ui = Square(0, 0, 100, 100);
        ui.Render();

        Texture sampleTextTexture;
        SDL_Color textColor {0x00, 0xFF, 0x00, 0xFF};
        sampleTextTexture.LoadFromRenderedText("Hello", textColor);
        sampleTextTexture.Render(0.f, sampleTextTexture.GetHeight());

        SDL_RenderPresent(System::sRenderer);
    }

    sys.Close();
}

