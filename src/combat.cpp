#include "pch.h"

#include "combat.h"
#include "entity.h"

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
