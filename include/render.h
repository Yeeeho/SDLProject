#pragma once

class RenderManager;

class RenderManager {
    public:
    void RenderThings();
    void AdjustFps(Timer& timer);
    
    int GetDesiredFps();

    private:

    bool mCapFpsOn = true;
    bool mVsyncOn = true;

    int mDesiredFps = 60;
    double mCurrentFps = 0.0;
};