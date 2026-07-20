#include "pch.h"

#include "util.h"

Uint32 Util::CallBack(void *param, SDL_TimerID timerID, Uint32 interval)
{
    return 0;
}

int Util::ClampLine(int min, int max, int x, int w)
{
    if (x < min) {
        x = min;
        std::string message = "hitting min limit at: " + std::to_string(x);
        SDL_Log(message.c_str());
    }
    else if (x + w > max){
        x = max - w;
        std::string message = "hitting max limit at: " + std::to_string(x);
        SDL_Log(message.c_str());
    } 
    return w;
}
