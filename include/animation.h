#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <queue>

class SpriteSheetManager;
class Texture;

class AnimationContext {
    public:
    AnimationContext(std::string hitSsName, SDL_FRect frect, int interval);
    std::string mHitSsName {""};
    SDL_FRect mFrect {0.f, 0.f, 0.f, 0.f};
    int mInterval {0};
};

class AnimationManager {
    public:
    AnimationManager(SpriteSheetManager* spsm);
    
    SpriteSheetManager* mSpsm {nullptr}; //레퍼런스
    
    std::queue<AnimationContext*> mAniCtxQueue;
    AnimationContext* mCurrentAniCtx {nullptr};
    int mCurrentIdx {0};

    bool mIsUpdating {false};
    bool mIsRender {false};
    bool mIsRenderUpdate {false};

    bool QueueAnimCtx(AnimationContext* animCtx);
    void UpdateAnimCtx();

    void RenderSprite();
};