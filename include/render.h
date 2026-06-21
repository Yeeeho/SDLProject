#pragma once

class RenderManager;
class UIManager;
class Timer;

class RenderManager {
    public:
    void RenderFps();

    void AdjustFps(Timer& timer);
    
    int GetDesiredFps();

    //ui 업데이트용 플래그 변수
    bool mIsUIUpdate = true;

    private:

    bool mCapFpsOn = true;
    bool mVsyncOn = true;

    int mDesiredFps = 60;
    double mCurrentFps = 0.0;
};