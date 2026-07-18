#include "pch.h"

#include <string>

#include "turn.h"
#include "map.h"
#include "entity.h"

OverMapTurnManager::OverMapTurnManager()
{
}

void OverMapTurnManager::SetOverMapTurn(int turn)
{
    mOverMapTurn = turn;

    std::string ovmt = std::to_string(mOverMapTurn);
    std::string  message = "set overmap turn to " + ovmt;
    SDL_Log(message.c_str());
}

void OverMapTurnManager::Update()
{
}

void OverMapTurnManager::TakeTurn(Entity *ent)
{
}

void OverMapTurnManager::IncOverMapTurn()
{
    mOverMapTurn += 1;
    if (mOverMapTurn >= mOverMapTurnCap) {
        SDL_Log("hitting over map turn cap");
        mOverMapTurn = 0;
    }

    std::string ovmt = std::to_string(mOverMapTurn);
    std::string message = "current overmap turn: " + ovmt;
    SDL_Log(message.c_str());
}

int OverMapTurnManager::GetOverMapTurn()
{
    return mOverMapTurn;
}

void SubMapTurnManager::EnterMap(Map* map)
{
    mCurrentMap = map;
}

void SubMapTurnManager::IncSubMapTurn()
{
    mSubMapTurn += 1;
    
    std::string sbmt = std::to_string(mSubMapTurn);
    std::string message = "current submap turn: " + sbmt;
    SDL_Log(message.c_str());
}

int SubMapTurnManager::GetSubMapTurn()
{
    return mSubMapTurn;
}

void SubMapTurnManager::SetSubMapTurn(int turn)
{
    mSubMapTurn = turn;
    
    std::string sbmt = std::to_string(mSubMapTurn);
    std::string message = "set submap turn to: " + sbmt;
    SDL_Log(message.c_str());
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

    Entity* target {nullptr};
    //아군이 턴을 모두 잡은 뒤에야 적이 턴을 잡는다.
    if (mPawnMaxNum != 0 && mPawnIdx != mPawnMaxNum) {
        SDL_Log("submap turn manager: target set to pawn");
        target = mCurrentMap->mPawns[mPawnIdx];
        mPawnIdx += 1;
    }
    else if (mNpcMaxNum != 0 && mNpcIdx != mNpcMaxNum) {
        SDL_Log("submap turn manager: target set to npc");
        target = mCurrentMap->mNpcs[mNpcIdx];
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

    
}

