#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <queue>

class SpriteSheetManager;
class Texture;

class AnimationContext {
    public:
    AnimationContext(SpriteSheetManager* spsm, std::string hitSsName, SDL_FRect frect, int interval);
    SpriteSheetManager* mSpsm {nullptr};

    std::string mHitSsName {""};
    SDL_FRect mFrect {0.f, 0.f, 0.f, 0.f}; //스프라이트가 위치할 FRect

    Texture* mSs {nullptr}; //스프라이트 시트
    nlohmann::json mSsMap; //스프라이트 시트의 메타데이터인 맵

    int mSpriteMaxIdx {0}; //최대 스프라이트 인덱스    
    int mSpriteIdx {0}; //스프라이트 시트에서의 인덱스

    //스프라이트 수명 관리
    bool mFirstTime {true};
    //TODO: 인터벌도 데이터로 빼야한다.
    int mInterval {0};
    Uint64 mInitTick {0};
    Uint64 mCurrentTick {0};
};

class AnimationManager {
    public:
    AnimationManager(SpriteSheetManager* spsm);
    
    SpriteSheetManager* mSpsm {nullptr}; //레퍼런스
    
    std::queue<AnimationContext*> mAniCtxQueue;
    AnimationContext* mCurrentAniCtx {nullptr};

    bool mIsUpdating {false};
    bool mIsRender {false};
    bool mIsRenderUpdate {false};

    bool QueueAnimCtx(AnimationContext* animCtx);
    void UpdateAnimCtx();

    void RenderAnimation(SDL_FRect& cam);
};