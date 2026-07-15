#pragma once

#include <SDL3/SDL.h>

class Camera {
    public:
    Camera(unsigned int velocity, int xlimit, int ylimit);

    SDL_FRect mSight; //시야 박스

    void HandleEvent(SDL_Event& e);

    unsigned int mVelocity {0};

    int mXSpeed {0};
    int mYSpeed {0};

    int mXLimit {0};
    int mYLimit {0};

    void Move();
};