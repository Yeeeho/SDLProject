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
    // if (mIsUpdating) return;

    // mIsUpdating = true;

    mCurrentAniCtx = mAniCtxQueue.front();
    mAniCtxQueue.pop();
}

//TODO: 그냥 컨텍스트 클래스에 렌더링 메서드를 넣는게 좋을거같습니다만?
//TODO: 동시에 여러 장소에 렌더링을 못하니까 그걸 어떻게 하도록.
void AnimationManager::RenderAnimation(SDL_FRect& cam)
{
    using json = nlohmann::json;

    if (!mCurrentAniCtx) return;

    AnimationContext* animCtx = mCurrentAniCtx;
    if (animCtx->mFirstTime) {
        //처음 렌더링되는 거라면 처음 렌더링된 시간을 저장하고 플래그를 초기화한다.
        animCtx->mInitTick = SDL_GetTicks();
        animCtx->mFirstTime = false;
    }
    //시간을 계속 캐싱한다.
    animCtx->mCurrentTick = SDL_GetTicks();
    
    //캐싱한 시간과 렌더링 시작 시간의 차이를 비교해서 의도한 인터벌 이상이라면 일단 지금 인덱스가 최대 인덱스인지 확인.
    if (animCtx->mCurrentTick - animCtx->mInitTick >= animCtx->mInterval) {
        
        //최대 인덱스가 지금 인덱스와 같다면 애니메이션을 종료한다.
        //그리고 컨텍스트도 삭제한다.
        if (animCtx->mSpriteIdx == animCtx->mSpriteMaxIdx) {
            delete mCurrentAniCtx;
            mCurrentAniCtx = nullptr;
            return;
        }
        //아직 최대 인덱스가 아니라면 인덱스를 증가시키고, 초기화 작업을 거쳐준다.
        animCtx->mSpriteIdx += 1;
        animCtx->mInitTick = SDL_GetTicks();
    }

    SDL_FRect target = mCurrentAniCtx->mFrect;

    std::string hitSsName = mCurrentAniCtx->mHitSsName;

    json sprites = animCtx->mSsMap["sprites"];

    json sp = sprites[animCtx->mSpriteIdx];

    SDL_FRect fr = {sp["x"].get<float>(), sp["y"].get<float>(),
        sp["width"].get<float>(), sp["height"].get<float>() 
    };

    animCtx->mSs->Render(target.x - cam.x, target.y - cam.y, &fr, target.w, target.h);
}

AnimationContext::AnimationContext(SpriteSheetManager* spsm, std::string hitSsName, SDL_FRect frect, int interval)
{
    using json = nlohmann::json;
    mSpsm = spsm;
    mHitSsName = hitSsName;
    mFrect = frect;
    mInterval = interval;

    mSs = mSpsm->GetSpriteSheet(hitSsName);
    mSsMap = mSpsm->GetSpriteMap(hitSsName);
    
    mSpriteMaxIdx = mSsMap["sprites"].back()["order"].get<int>(); //애니메이션의 최대 프레임을 확인하고 캐싱 뭐 일단 최대 인덱스라고 하긴했다.
}
