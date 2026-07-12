#pragma once

class RenderManager;
class UIManager;
class Texture;
class Timer;

class RenderManager {
    public:
    void RenderFps();

    void AdjustFps(Timer& timer);
    
    int GetDesiredFps();

    void SetRenderTarget(SDL_Renderer*, SDL_Texture*);

    private:
    bool mCapFpsOn = true;
    bool mVsyncOn = true;

    int mDesiredFps = 60;
    double mCurrentFps = 0.0;
};