#include "pch.h"

#include <string>

#include "game_context.h"
#include "ui.h"
#include "turn.h"
#include "combat.h"
#include "map.h"
#include "entity.h"

TurnManager::TurnManager(GameContext *gc)
{
    mGc = gc;
}

void TurnManager::Enter(Map *map)
{
    mCurrentMap = map;
    UpdateTurn();
}

void TurnManager::Update()
{
}

void TurnManager::IncTurn()
{
}

void TurnManager::IncCurMapTurn()
{
    mCurrentMap->mCurrentTurn += 1;
    
    std::string sbmt = std::to_string(mSubMapTurn);
    std::string message = "current submap turn: " + sbmt;
    SDL_Log(message.c_str());
}

int TurnManager::GetCurMapTurn()
{
    return mCurrentMap->mCurrentTurn;
}

void TurnManager::IncGlobalTurn()
{
    mGlobalTurn += 1;
    
    std::string gt = std::to_string(mGlobalTurn);
    std::string message = "current global turn: " + gt;
    SDL_Log(message.c_str());
}

int TurnManager::GetGlobalTurn()
{
    return mGlobalTurn;
}

void TurnManager::TakeTurn(Entity *ent)
{
    mCurrentTarget = ent;
    mPrevTarget = ent;

    SDL_Color tc;
    if (ent->mDemeanor == Demeanor::Friendly) tc = {0x40, 0xB0, 0x40, 0xFF};
    else if (ent->mDemeanor == Demeanor::Hostile) tc = {0xB0, 0x40, 0x40, 0xFF};
    else if (ent->mDemeanor == Demeanor::Neutral) tc = {0xB0, 0xB0, 0x40, 0xFF};

    std::string message = ent->mName + "의 차례입니다!";
    mGc->mUim->mLogUI->AddMessage(message, tc);

    //턴당 회복
    CombatHelper ch;
    ch.RegenEntity(ent, *mGc);
}

void TurnManager::ClearTargets()
{
    mCurrentTarget = nullptr;
    mPrevTarget = nullptr;
}

void TurnManager::UpdateEntityQueue()
{
    if (!mIsQueueUpdate) return;

    SDL_Log("submap turn manager: updating entity queue");

    mIsQueueUpdate = false;
}

void TurnManager::UpdateTurn()
{
    //맵에 아무도 없는 경우
    if (mCurrentMap->mPawns.empty() && mCurrentMap->mNpcs.empty()) {
        SDL_Log("nobody's existing on map!");
        return;
    }

    Entity* target {nullptr};
    //pc가 턴을 모두 잡은 뒤에야 적이 턴을 잡는다.
    //TODO: 플레이어가 임의로 큐를 설정할 수 있게 만들어야함
    while (true) {
        if (mPawnIdx < (int) mCurrentMap->mPawns.size()) {
            //pc가 턴을 잡음
            IncTurn();
            SDL_Log("turn manager: target set to pawn");
            target = mCurrentMap->mPawns[mPawnIdx];
            mPawnIdx += 1;
            TakeTurn(target);
            break;
        }
        else if (mNpcIdx < (int) mCurrentMap->mNpcs.size()) {
            //npc가 턴을 잡음
            IncTurn();
            SDL_Log("turn manager: target set to npc");
            target = mCurrentMap->mNpcs[mNpcIdx];
            mNpcIdx += 1;
            TakeTurn(target);
            break;
        } 
        else {
            //모두가 턴을 잡음
            mPawnIdx = 0;
            mNpcIdx = 0;
        }
    }
}
