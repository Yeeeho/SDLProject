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
    namespace eh = EntityHelper;
    LogUI* log = gc.mUim->mLogUI;
    // SDL_Color w = {0xB0, 0xB0, 0xB0, 0xFF};
    int armor = sh.GetTotalArmor(ent);

    damageInput -= armor;
    ent->mCurHp -= damageInput;

    std::string message = ent->mName + "는 " + std::to_string(damageInput) + "체력 데미지를 받았다!";
    log->AddMessage(message, eh::GetRDemeanorColor(ent));

    if (DeathCheck(ent)) {
        std::string message = ent->mName + "이(가) 죽었습니다!";
        log->AddMessage(message, eh::GetRDemeanorColor(ent));
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

void CombatHelper::RegenEntity(Entity *ent, GameContext& gc)
{
    StatHelper sh;
    int hpregen = sh.GetHpRegen(ent);
    int spregen = sh.GetSpRegen(ent);
    int apregen = sh.GetApRegen(ent);
    int mxhp = sh.GetMaxHp(ent);
    int mxsp = sh.GetMaxSp(ent);
    int mxap = sh.GetMaxAp(ent);
    
    std::string message {""};
    ent->mCurHp += hpregen;
    message = std::to_string(hpregen) + " 만큼 HP를 회복했습니다.";
    SDL_Log(message.c_str());
    if (ent->mCurHp > mxhp) ent->mCurHp = mxhp; 

    ent->mCurSp += spregen;
    message = std::to_string(spregen) + " 만큼 SP를 회복했습니다.";
    SDL_Log(message.c_str());
    if (ent->mCurSp > mxsp) ent->mCurSp = mxsp;

    ent->mCurAp += apregen;
    message = std::to_string(apregen) + " 만큼 AP를 회복했습니다.";
    SDL_Log(message.c_str());
    if (ent->mCurAp > mxap) ent->mCurAp = mxap;
    
    gc.mUim->mBCUI->UpdateUI(ent);
    gc.mUim->mToolTip->mIsRenderUpdate = true;
}
