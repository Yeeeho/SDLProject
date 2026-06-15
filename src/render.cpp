#include "pch.h"

#include "system.h"
#include "ui.h"
#include "render.h"
#include "square.h"
#include "texture.h"

//렌더링 짬통
void RenderManager::RenderThings(UIManager uim)
{
    bool test = true;

    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(System::sRenderer);

    Square uiFrame = Square(10, 10, 300, 300);
    UI ui = UI(&uiFrame, "로로로로롤 로로롤 로로롤");
    ui.Render();

    uim.RenderUIs();

    //fps표시
    Timer timer;
    Texture sampleTextTexture;
    SDL_Color textColor {0x00, 0xB0, 0x00, 0xFF};
    sampleTextTexture.LoadFromRenderedText(std::to_string((mCurrentFps)), 0, textColor);
    sampleTextTexture.Render(System::sWindowWidth - sampleTextTexture.GetWidth() - 10.f, 10.f);

    SDL_RenderPresent(System::sRenderer);
}

//렌더링 루프에 하나만 쓰세요
void RenderManager::AdjustFps(Timer& timer)
{
    Uint64 prevNs = timer.GetPreviousTick();
    // SDL_Log("Prev: ");
    Uint64 currentNs = timer.StoreProgramTick();
    // SDL_Log("current: ");

    Uint64 desiredTickPerFrame = 1000000000 / mDesiredFps;
    Uint64 currentTickPerFrame = currentNs - prevNs;

    Uint64 tickDiff = desiredTickPerFrame - currentTickPerFrame;

    if (mCapFpsOn && desiredTickPerFrame > currentTickPerFrame) {
        SDL_DelayNS(tickDiff);
    }

    mCurrentFps = 1000000000.0 / static_cast<double>(SDL_GetTicksNS() - prevNs);
}

int RenderManager::GetDesiredFps()
{
    return mDesiredFps;
}
