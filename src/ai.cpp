#include "pch.h"

#include "ai.h"
#include "game_object.h"
#include "game_context.h"
#include "system/game_json.h"
#include "system/game_random.h"
#include "map.h"
#include "entity.h"
#include "skill/skill.h"
#include "move.h"

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

void AIState::NavigateQueue(Npc *npc, Map* map, int targetTileId)
{
    using namespace std;
    namespace mh = MapHelper;
    string message {""};

    Skill* moveskill = SkillHelper::GetMovementSkill(npc);
    if (!moveskill) {
        message = npc->mName + "couldn't navigate to" + to_string(targetTileId);
        SDL_Log(message.c_str());
        SDL_Log("why: no movement skill");
        return;
    }
    vector<int> tids = mh::GetTilesIdBetween(map, npc->mTileId, targetTileId);
    tids.pop_back(); //엔티티가 서있을 타일은 제외한다.

    SkillContext* skctx = new SkillContext(moveskill, tids, map);
    npc->mSkillCtxQueue.push(skctx);
    SDL_Log("navigate queued");
}

IdleState::IdleState(GameContext *gc)
{
    mGc = gc;
}

void IdleState::Destroy()
{
    mGc = nullptr;
}

//TODO: 랜덤 이동도 나중에 써먹을수 있으니 재사용을 염두해라
void IdleState::UpdateSkillQueue(Npc* npc)
{
    SDL_Log("ai idle state: update skill queue");
    
    //이동 스킬이 없다면
    Skill* currentSkill = SkillHelper::GetMovementSkill(npc);
    if (!currentSkill) return;

    //이동 스킬이 있다면
    //이미 스킬 큐에 무언가 있다면 반환
    if (!npc->mSkillCtxQueue.empty()) return;

    int num = Random::IDistribution(0, 8);
    //정지
    if (num == 0) return;

    std::vector<int> tileIds;
    Map* currentMap = mGc->mMapm->mCurrentMap;
    namespace mh = MapHelper;
    Point p = {0, 0};

    using namespace std;
    std::string message {""};
    if (mPrevTileId == -1) {
        tileIds.push_back(npc->mTileId);
        message = "update skill queue: npc moving from tile id: " + to_string(npc->mTileId);
        p = mh::GetPosPoint(npc->mTileId, currentMap);    
    }
    else {
        tileIds.push_back(mPrevTileId);
        message = "update skill queue: npc moving from tile id: " + to_string(mPrevTileId);
        p = mh::GetPosPoint(mPrevTileId, currentMap);
    }
    SDL_Log(message.c_str());

    if (num == 1) p.mX -= 1; p.mY -= 1;  
    if (num == 2) p.mY -= 1;
    if (num == 3) p.mX += 1; p.mY -= 1;
    if (num == 4) p.mX += 1;
    if (num == 5) p.mX += 1; p.mY += 1;
    if (num == 6) p.mY += 1;
    if (num == 7) p.mX -= 1; p.mY += 1;
    if (num == 8) p.mX -= 1;

    int targetTid = mh::WhatTileOnPoint(p, currentMap);
    message = "update skill queue: npc moving to tile id: " + to_string(targetTid);
    SDL_Log(message.c_str());

    mPrevTileId = targetTid;
    tileIds.push_back(targetTid);

    json skillTable = mGc->mSkm->mSkillDb["items"];
    json sd = skillTable[currentSkill->mCode];

    SkillContext* skctx = new SkillContext(currentSkill, tileIds, currentMap);
    npc->mSkillCtxQueue.push(skctx);
}

CombatState::CombatState(GameContext *gc)
{
    mGc = gc;
}

void CombatState::UpdateSkillQueue(Npc* npc)
{
    SDL_Log("update skill queue: combat");
    json aidata = AIHelper::GetAIData(mGc, npc);
    //세 가중치는 합쳐서 1이 되어야 한다.
    float atkw = aidata["attack_weight"].get<float>();
    float defw = aidata["defense_weight"].get<float>();
    float movw = aidata["move_weight"].get<float>();

    Map* map = mGc->mMapm->mCurrentMap;
    if (map->mPawns.empty()) return;
    Entity* p = map->mPawns[0];
    
    npc->ClearSkCtxQueue();

    //현재는 근접전만을 상정해서 스킬 큐를 업데이트 한다.
    NavigateQueue(npc, map, p->mTileId);
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

void AI::TakeTurn(GameContext* gctx, Npc* npc)
{
    SDL_Log("ai taketurn");
    while (!npc->mSkillCtxQueue.empty()) {
        npc->mSkillCtxQueue.front()->Activate(gctx, npc);
        delete npc->mSkillCtxQueue.front();
        npc->mSkillCtxQueue.pop();
    }

    mCurrentState->mPrevTileId = -1;
}

void AI::UpdateSkillQueue(GameContext* gctx, Npc* npc)
{
    mCurrentState = Transition(gctx, npc);
    mCurrentState->UpdateSkillQueue(npc);
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
