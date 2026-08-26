#include "pch.h"

#include "ai.h"
#include "game_context.h"
#include "game_random.h"
#include "map.h"
#include "entity.h"
#include "skill/skill.h"

void AIState::Destroy()
{
    mGc = nullptr;
}

void AIState::Enter()
{
}

void AIState::Exit()
{
}

void AIState::TakeTurn(Entity *ent)
{
}

IdleState::IdleState(GameContext *gc)
{
    mGc = gc;
}

void IdleState::Destroy()
{
    mGc = nullptr;
}

void IdleState::TakeTurn(Entity *ent)
{
    Skill* currentSkill = nullptr;
    for (Skill* skill : ent->mSkills) {
        if (skill->mCode == "move") {
            currentSkill = skill;
        }
    }

    //이동 스킬이 없다면
    if (!currentSkill) return;
    //이동 스킬이 있다면

    int num = Random::IDistribution(0, 8);
    //정지
    if (num == 0) return;

    std::vector<int> tileIds;
    tileIds.push_back(ent->mTileId);
    Map* currentMap = mGc->mMapm->mCurrentMap;
    MapHelper mh;
    Point p = mh.GetPosPoint(ent->mTileId, currentMap);
    
    if (num == 1) p.mX -= 1; p.mY -= 1;  
    if (num == 2) p.mY -= 1;
    if (num == 3) p.mX += 1; p.mY -= 1;
    if (num == 4) p.mX += 1;
    if (num == 5) p.mX += 1; p.mY += 1;
    if (num == 6) p.mY += 1;
    if (num == 7) p.mX -= 1; p.mY += 1;
    if (num == 8) p.mX -= 1;

    int targetTid = mh.WhatTileOnPoint(p, currentMap);
    tileIds.push_back(targetTid);

    json skillTable = mGc->mSkm->mSkillDb["items"];
    json sd = skillTable[currentSkill->mCode];

    SkillManager* skm = mGc->mSkm;
    skm->SetActor(ent);
    skm->SetMap(mGc->mMapm->mCurrentMap);
    skm->SetSkill(currentSkill);
    skm->SetTileIds(tileIds);
    skm->ActivateSkill();
}

CombatState::CombatState(GameContext *gc)
{
    mGc = gc;
}

void CombatState::TakeTurn(Entity *ent)
{
}

FleeState::FleeState(GameContext *gc)
{
    mGc = gc;
}

void FleeState::TakeTurn(Entity *ent)
{
}

AI::AI(GameContext* gc)
{
    mCurrentState = new IdleState(gc);

    mGc = gc;
}

AI::~AI()
{
    Destroy();
}

void AI::Destroy()
{
    delete mCurrentState;
    mGc = nullptr;
}

void AI::TakeTurn(Entity *ent)
{
    mCurrentState->TakeTurn(ent);
}
