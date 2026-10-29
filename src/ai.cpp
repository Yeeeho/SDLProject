#include "pch.h"

#include "math.h"
#include "shape/point.h"
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

//WORK IN PROGRESS
void AIState::NavigateQueue(Npc *npc, Map* map, int targetTileId)
{
    using namespace std;
    namespace mh = MapHelper;
    string message {""};

    Skill* moveskill = SkillHelper::GetMovementSkill(npc);
    if (!moveskill) {
        message = "[INFO] " + npc->mName + "couldn't navigate to" + to_string(targetTileId);
        SDL_Log(message.c_str());
        SDL_Log("why: no movement skill");
        return;
    }
    
    //npc와 타겟 사이의 직선거리 타일들을 구함

    int apPerTile = StatHelper::GetApPerTileMove(npc);

    //DEBUGGING
    //액터를 기준으로 타겟의 방향을 산출하고 방향을 기준으로 다음 타일 아이디를 구한다.
    
    //예약을 초기화하고 시뮬레이션으로 경로를 설정한 뒤 최종 목적지의 타일은 예약 상태로 변경한다.

    map->mMapTiles[npc->mReservedTid]->mIsReserved = false; //이전 타일 예약 초기화
    SDL_Log("[INFO] Navigate skill queue: tile unreserved, tile id: %d", npc->mReservedTid);
    
    //한칸씩 시뮬레이션 돌린다
    std::vector<int> tids; //스킬 컨텍스트에 전달하기 위한 타일 아이디 컨테이너다.

    int nextTID = AIMoveHelper::GetNextTID(mGc, map, npc->mTileId, targetTileId); //초기 값
    tids.push_back(nextTID); //일단 넣어

    while (true) {
        mVirtualAp -= apPerTile;
        if (mVirtualAp < 0) break;

        nextTID = AIMoveHelper::GetNextTID(mGc, map, nextTID, targetTileId);

        if (tids.back() != nextTID) tids.push_back(nextTID); //중복되지 않았다면 넣어준다.
        SDL_Log("[INFO] next TID: %d", nextTID);
    }

    map->mMapTiles[nextTID]->mIsReserved = true; //최신 목적지 타일 예약
    npc->mReservedTid = nextTID;
    SDL_Log("[INFO] Navigate skill queue: tile reserved, tile id: %d", nextTID);

    //스킬 큐에 푸시하는 단계
    SkillContext* skctx = new SkillContext(moveskill, npc, tids, map);
    npc->mSkillCtxQueue.push(skctx);
    SDL_Log("navigate queued");
}

IdleState::IdleState(GameContext *gc)
{
    mGc = gc;
}

IdleState::~IdleState()
{
    Destroy();
}

void IdleState::Destroy()
{
    mGc = nullptr;
}

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

    json& skillTable = mGc->mSkm->mSkillDb["items"];
    json sd = skillTable[currentSkill->mCode];

    SkillContext* skctx = new SkillContext(currentSkill, tileIds, currentMap);
    npc->mSkillCtxQueue.push(skctx);
}

CombatState::CombatState(GameContext *gc)
{
    mGc = gc;
}

CombatState::~CombatState()
{
    Destroy();
}

void CombatState::Destroy()
{
    mGc = nullptr;
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

FleeState::~FleeState()
{
    Destroy();
}

void FleeState::Destroy()
{
    mGc = nullptr;
}

void FleeState::UpdateSkillQueue(Npc* npc)
{
}

AI::AI(GameContext* gc)
{
    mGc = gc;
    
    mIdleState = new IdleState(gc);
    mCombatState = new CombatState(gc);
    mFleeState = new FleeState(gc);

    mCurrentState = mIdleState;
}

AI::~AI()
{
    Destroy();
}

void AI::Destroy()
{
    delete mIdleState;
    delete mCombatState;
    delete mFleeState;
    mGc = nullptr;
}

void AI::Transition(GameContext* gctx, Npc* npc)
{
    json aidata = AIHelper::GetAIData(gctx, npc);
    
    float fleeHpMod = aidata["flee_hp"].get<float>();
    if (npc->mCurHp < StatHelper::GetMaxHp(npc) * fleeHpMod) {
        mCurrentState = mFleeState;
    }

    if (npc->mDemeanor == Demeanor::Hostile) {
        mCurrentState = mCombatState;
    }

    else {
        mCurrentState = mIdleState;
    }
}

void AI::TakeTurn(GameContext* gctx, Npc* npc)
{
    SDL_Log("ai taketurn");

    gctx->mObjm->mUpdateInQueue = true;
    while (!npc->mSkillCtxQueue.empty()) {
        gctx->mObjm->mSkCtxUpdateQueue.push(npc->mSkillCtxQueue.front());

        npc->mSkillCtxQueue.pop();
    }

    mCurrentState->mPrevTileId = -1;
}

void AI::UpdateSkillQueue(GameContext* gctx, Npc* npc)
{
    Transition(gctx, npc);

    mCurrentState->mVirtualAp = npc->mCurAp;
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

//TODO: 목표 타일로 이동할수 없다면 우회하여 차순위 타일을 찾도록 만들어야 함.
int AIMoveHelper::GetNextTID(GameContext *gctx, Map *map, int actorTID, int targetTID)
{
    Vector2 vec = GetVectorFromTID(map, actorTID, targetTID);
    SDL_Log("[INFO] Get final melee TID: vector2: %d, %d", vec.x, vec.y);

    int dirChecksum {0}; //체크섬용 방향 배열
    Direction dir = Math().GetDirection8(vec.x, vec.y); //초기 방향을 구한다.
    Direction8 dir8 = Direction8(dir); //초기 방향 객체를 생성한다.

    //체크섬을 기반으로 이동 가능 방향을 하나씩 소거한다.
    while (dirChecksum < 8) {
        
        //이미 근접해 있다면 굳이 연산하지 않는다.
        if (vec.x <= 1 && vec.y <= 1 && vec.x >= -1 && vec.y >= -1) {
            SDL_Log("--> Target already adjacent");
            return actorTID;
        }
    
        //액터와 타겟 사이의 벡터를 구해 진행 방향을 결정한다.

        //현재 방향을 기준으로 타일을 구하고 이동 가능 여부를 확인
        int nextTID = MapHelper::GetAdjacentTID(map, dir8.mDir, actorTID);
    
        if (map->mMapTiles[nextTID]->IsPassable()) {
            //이동 가능할 경우
            SDL_Log("--> Passable");
            return nextTID;
        }
        else {
            SDL_Log("--> Not Passable");
            SDL_Log("--> Current direction: %d", (int) dir8.mDir);
            dirChecksum += 1;
            //DEBUGGIN 지금은 시계방향으로만 경로를 우회한다.
            //TODO: 랜덤으로 시계 혹은 반시계로 탐색하도록 수정한다.
            dir8.GoCW();
            SDL_Log("--> Direction set to clockwise, direction: %d", (int) dir8.mDir);            
        } 
    }

    //반복문을 다 돌았을 경우 이동할 수 없었다는 뜻이므로 액터가 서있는 타일 아이디를 반환한다.
    SDL_Log("--> No passable tile nearby");
    return actorTID;
}

Vector2 AIMoveHelper::GetVectorFromTID(Map *map, int actorTID, int targetTID)
{
    Point p1 = MapHelper::GetPosPoint(actorTID, map);
    Point p2 = MapHelper::GetPosPoint(targetTID, map);

    int xdiff = p2.mX - p1.mX;
    int ydiff = p2.mY - p1.mY;

    return Vector2{xdiff, ydiff};
}
