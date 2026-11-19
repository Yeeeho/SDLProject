#include "pch.h"

#include "texture.h"
#include "animation.h"

AnimationManager::AnimationManager(SpriteSheetManager *spsm)
{
    mSpsm = spsm;
}

bool AnimationManager::QueueAnimCtx(AnimationContext *animCtx)
{
    mAniCtxQueue.push(animCtx);
    return true;
}

void AnimationManager::UpdateAnimCtx()
{
    if (mAniCtxQueue.empty()) return; //큐가 비었으면 반환한다.
    if (mIsUpdating) return;

    mIsUpdating = true;

    mCurrentAniCtx = mAniCtxQueue.front();
    mAniCtxQueue.pop();
}

void AnimationManager::RenderSprite()
{
    using json = nlohmann::json;

    if (!mCurrentAniCtx) return;

    SDL_FRect target = mCurrentAniCtx->mFrect;

    std::string hitSsName = mCurrentAniCtx->mHitSsName;
    Texture* ss = mSpsm->GetSpriteSheet(hitSsName);
    json ssMap = mSpsm->GetSpriteMap(hitSsName);

    json sprites = ssMap["sprites"];

    json sp = sprites[mCurrentIdx];

    SDL_FRect fr = {sp["x"].get<float>(), sp["y"].get<float>(),
        sp["width"].get<float>(), sp["height"].get<float>() 
    };

    ss->Render(target.x, target.y, &fr, target.w, target.h);
    SDL_Log("dddd");
}

AnimationContext::AnimationContext(std::string hitSsName, SDL_FRect frect, int interval)
{
    mHitSsName = hitSsName;
    mFrect = frect;
    mInterval = interval;
}
