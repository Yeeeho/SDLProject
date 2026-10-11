#include "pch.h"

#include "game_context.h"
#include "game_object.h"
#include "system/game_json.h"
#include "item/item.h"
#include "item/item_manager.h"
#include "skill/skill.h"
#include "skill/skill_enum.h"
#include "ui.h"
#include "text.h"
#include "city.h"
#include "entity.h"
#include "map.h"
#include "system/system.h"

ObjectManager::ObjectManager(GameContext* gc)
{
    mGc = gc;
    mJsm = new JsonManager();
    mItm = new ItemManager(gc);
}

void ObjectManager::DestroyObjects()
{
    //오브젝트 파괴하는 함수. 아직 미구현.
}

void ObjectManager::UpdateObjects()
{
    UpdateEntities(mGc);
    UpdateSkCtx();
}

void ObjectManager::UpdateEntities(GameContext *gc)
{
    if (mSkCtxUpdateQueue.empty()) return; //스킬 컨텍스트 큐가 비었으면 반환함
    if (mIsUpdating) return; //업데이트 중이라면 반환함

    mIsUpdating = true;

    mCurrentSkCtx = mSkCtxUpdateQueue.front();
    mSkCtxUpdateQueue.pop();

    SDL_Log("[INFO] updated entity");
}

void ObjectManager::UpdateSkCtx()
{
    if (!mCurrentSkCtx) return; //현재 컨텍스트가 없으면 반환

    bool success = mCurrentSkCtx->Activate(mGc);
    //이동에 실패했을 경우 시마이친다.
    if (success == false) {
        delete mCurrentSkCtx;
        mCurrentSkCtx = nullptr;
        mIsUpdating = false; //업데이트 완료
    }

    //이동에 성공했다면 다음 업데이트 루프에 다시 이 메서드를 실행.
    SDL_Log("[INFO] updated skctx");
}
