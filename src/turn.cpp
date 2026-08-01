#include "pch.h"

#include <string>

#include "turn.h"
#include "map.h"
#include "entity.h"

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
}

void TurnManager::UpdateEntityQueue()
{
    if (!mIsQueueUpdate) return;

    SDL_Log("submap turn manager: updating entity queue");

    mIsQueueUpdate = false;
}

void TurnManager::UpdateTurn()
{
    Entity* target {nullptr};
    //pc가 턴을 모두 잡은 뒤에야 적이 턴을 잡는다.
    if (mPawnIdx < mCurrentMap->mPawns.size()) {
        //pc가 턴을 잡음
        IncTurn();
        SDL_Log("turn manager: target set to pawn");
        target = mCurrentMap->mPawns[mPawnIdx];
        mCurrentTarget = target;
        mPrevTarget = target; //캐싱
        mPawnIdx += 1;
    }
    else if (mNpcIdx < mCurrentMap->mNpcs.size()) {
        //npc가 턴을 잡음
        IncTurn();
        SDL_Log("turn manager: target set to npc");
        target = mCurrentMap->mNpcs[mNpcIdx];
        mCurrentTarget = target;
        mPrevTarget = target;
        mNpcIdx += 1;
    } 
    else {
        //모두가 턴을 잡음
        mPawnIdx = 0;
        mNpcIdx = 0;
        //맵에 아무도 없는 경우
        if (mCurrentMap->mPawns.empty() && mCurrentMap->mNpcs.empty()) {
            SDL_Log("nobody's existing on map!");
            return;
        }
        //아직 누가 살아있는경우
        UpdateTurn(); //재귀모띠
    }
    TakeTurn(target);
}
