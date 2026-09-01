#include "pch.h"

#include "ai.h"
#include "game_object.h"
#include "game_context.h"
#include "system/game_json.h"
#include "system/game_random.h"
#include "map.h"
#include "entity.h"
#include "skill/skill.h"

using json = nlohmann::json;

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

void AIState::UpdateSkillQueue(Npc* npc)
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

void IdleState::UpdateSkillQueue(Npc* npc)
{
    Skill* currentSkill = nullptr;
    for (Skill* skill : npc->mSkills) {
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
    tileIds.push_back(npc->mTileId);
    Map* currentMap = mGc->mMapm->mCurrentMap;
    MapHelper mh;
    Point p = mh.GetPosPoint(npc->mTileId, currentMap);
    
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
    skm->SetActor(npc);
    skm->SetMap(mGc->mMapm->mCurrentMap);
    skm->SetSkill(currentSkill);
    skm->SetTileIds(tileIds);
    // skm->ActivateSkill();

    skm->SetSkillContext(npc);
}

CombatState::CombatState(GameContext *gc)
{
    mGc = gc;
}

void CombatState::UpdateSkillQueue(Npc* npc)
{
    json aidata = AIHelper::GetAIData(mGc, npc);
    //세 가중치는 합쳐서 1이 되어야 한다.
    float atkw = aidata["attack_weight"].get<float>();
    float defw = aidata["defense_weight"].get<float>();
    float movw = aidata["move_weight"].get<float>();

    //캐릭터 밸류가 낮은 적부터 공격한다.
}

FleeState::FleeState(GameContext *gc)
{
    mGc = gc;
}

void FleeState::UpdateSkillQueue(Npc* npc)
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

AIState *AI::Transition(GameContext* gctx, Npc* npc)
{
    json aidata = AIHelper::GetAIData(gctx, npc);
    
    float fleeHpMod = aidata["flee_hp"].get<float>();
    if (npc->mCurHp < StatHelper::GetMaxHp(npc) * fleeHpMod) {
        return new FleeState(gctx);
    }

    if (npc->mDemeanor == Demeanor::Hostile) {
        return new CombatState(gctx);
    }

    else return new IdleState(gctx);
}

void AI::TakeTurn(Npc* npc)
{
    mCurrentState = Transition(mGc, npc);
    mCurrentState->UpdateSkillQueue(npc);
}

void AI::UpdateSkillQueue(Npc *npc)
{
}

json AIHelper::GetAIData(GameContext *gctx, Entity *ent)
{
    json& entdata = gctx->mObjm->mJsm->mEntDb["items"][ent->mCode];
    std::string kin = entdata["kin"].get<std::string>();

    json& aidata = gctx->mObjm->mEntm->mAim->mAIDb["items"][kin];
    return aidata;
}

int AIHelper::GetThreatValue(Entity *ent)
{
    int stat = 0;
    stat += ent->mStr;
    stat += ent->mEnd;
    stat += ent->mDex;
    stat += ent->mPer;
    stat += ent->mAgi;
    stat += ent->mWil;
    stat += ent->mInt;
    stat += ent->mSpd;

    return stat;
}

AIManager::AIManager()
{
    JsonHelper::LoadJsonFile(mAIDb, "data/ai/ai.json");
}
