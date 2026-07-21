#include "pch.h"

#include <string>

#include "turn.h"
#include "map.h"
#include "entity.h"

OverMapTurnManager::OverMapTurnManager()
{
}

void OverMapTurnManager::IncTurn()
{
    mCurrentTurn += 1;
    if (mCurrentTurn >= mTurnCap) {
        SDL_Log("hitting over map turn cap");
        mCurrentTurn = 0;
    }

    std::string ovmt = std::to_string(mCurrentTurn);
    std::string message = "current overmap turn: " + ovmt;
    SDL_Log(message.c_str());
}

void OverMapTurnManager::Update()
{
}

void OverMapTurnManager::TakeTurn(Entity *ent)
{
}

void SubMapTurnManager::EnterMap(Map* map)
{
    mCurrentMap = map;
}

void SubMapTurnManager::Update()
{
    UpdateEntityQueue();
    UpdateTurn();
}

TurnManager::~TurnManager()
{
}

void TurnManager::Update()
{
}

void TurnManager::IncTurn()
{
    mCurrentTurn += 1;
    
    std::string sbmt = std::to_string(mCurrentTurn);
    std::string message = "current submap turn: " + sbmt;
    SDL_Log(message.c_str());
}

int TurnManager::GetTurn()
{
    return mCurrentTurn;
}

void TurnManager::SetTurn(int turn)
{
    mCurrentTurn = turn;
    
    std::string sbmt = std::to_string(mCurrentTurn);
    std::string message = "set submap turn to: " + sbmt;
    SDL_Log(message.c_str());
}

void TurnManager::TakeTurn(Entity *ent)
{
}

void TurnManager::UpdateEntityQueue()
{
    if (!mIsQueueUpdate) return;

    SDL_Log("submap turn manager: updating entity queue");
    mPawnMaxNum = mCurrentMap->mPawns.size();
    mNpcMaxNum = mCurrentMap->mNpcs.size();

    mIsQueueUpdate = false;
}

void TurnManager::UpdateTurn()
{
    if (!mIsTurnUpdate) return;
    //널포인터가 아닐 경우 턴 플래그 초기화
    if (mPrevTarget) mPrevTarget->mIsTakingTurn = false; 

    Entity* target {nullptr};
    //pc가 턴을 모두 잡은 뒤에야 적이 턴을 잡는다.
    if (mPawnMaxNum != 0 && mPawnIdx != mPawnMaxNum) {
        //pc가 턴을 잡음
        SDL_Log("submap turn manager: target set to pawn");
        target = mCurrentMap->mPawns[mPawnIdx];
        mCurrentTarget = target;
        mPrevTarget = target; //캐싱
        mPawnIdx += 1;
    }
    else if (mNpcMaxNum != 0 && mNpcIdx != mNpcMaxNum) {
        //npc가 턴을 잡음
        SDL_Log("submap turn manager: target set to npc");
        target = mCurrentMap->mNpcs[mNpcIdx];
        mCurrentTarget = target;
        mPrevTarget = target;
        mNpcIdx += 1;

        if (mNpcIdx == mNpcMaxNum) {
            //모든 엔티티들을 순회했다.
            mPawnIdx = 0;
            mNpcIdx = 0;
        }
    } 
    else {
        SDL_Log("nobody's existing on map!");
        mIsTurnUpdate = false;
        return;
    }
    TakeTurn(target);

    mIsTurnUpdate = false;
}

void SubMapTurnManager::TakeTurn(Entity *ent)
{
    std::string message = ent->mName + " is taking turn";
    SDL_Log(message.c_str());
    ent->mIsTakingTurn = true;
}

