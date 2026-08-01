#include "pch.h"

#include "game_context.h"
#include "map.h"
#include "game_object.h"
#include "ui.h"
#include "entity.h"
#include "combat.h"

void CombatManager::EnterCombatState()
{
    mIsCombatState = true;
    SDL_Log("enter combat state");
}

void CombatManager::ExitCombatState()
{
    mIsCombatState = false;
    SDL_Log("exit combat state");
}

void CombatManager::Update(UIManager &uim, ObjectManager &objm)
{
    if (!mIsUpdate) return;
    SDL_Log("update combat manager");

    mIsUpdate = false;
}

void CombatHelper::TakeDamage(Entity *ent, GameContext& gc, int damageInput)
{
    StatHelper sh;
    int armor = sh.GetTotalArmor(ent);

    damageInput -= armor;
    ent->mCurHp -= damageInput;

    std::string message = ent->mName + "는 " + std::to_string(damageInput) + "체력 데미지를 받았다!";
    SDL_Log(message.c_str());

    if (DeathCheck(ent)) {
        std::string message = ent->mName + "이(가) 죽었습니다!";
        SDL_Log(message.c_str());
        gc.mObjm->mEntm->KillEntityOnMap(gc, gc.mMapm->mCurrentMap, ent);
    }
}

bool CombatHelper::DeathCheck(Entity *ent)
{
    bool isDead = false;

    if (ent->mCurHp <= 0) {
        isDead = true;
    } 

    return isDead;
}
