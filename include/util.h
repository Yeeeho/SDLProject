#pragma once

#include <SDL3/SDL.h>

class Util {
    public:
    static Uint32 CallBack(void* param, SDL_TimerID timerID, Uint32 interval);
};