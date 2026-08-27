#include "pch.h"

#include "math.h"
#include "render.h"
#include "system.h"
#include "game_context.h"
#include "game_json.h"
#include "game_object.h"
#include "ui.h"
#include "event_context.h"
#include "map.h"
#include "entity.h"
#include "camera.h"
#include "text.h"
#include "texture.h"
#include "item/item_manager.h"
#include "item/item.h"
#include "skill/skill.h"
#include "skill/skill_enum.h"
#include "ai.h"
#include "util.h"

using json = nlohmann::json;

Team::Team(std::string path, int id)
{
    mId = id;

    mTeamTex = new Texture();
    if (mTeamTex->LoadFromFile(path) == false) {
        std::string message = path + " not loaded";
        SDL_Log(message.c_str());
    }
}

PawnTeam::PawnTeam(std::string path, int id)
{
    mId = id;

    mTeamTex = new Texture();
    if (mTeamTex->LoadFromFile(path) == false) {
        std::string message = path + " not loaded";
        SDL_Log(message.c_str());
    }
}

TeamManager::TeamManager()
{
    for (int i = 0; i <(int)TeamSetting::MaxTeam;  i++) {
        mTeamTable[i] = new Team("images/entity/team/null_team.png", i);
        mTeamTable[i]->mName = "null_team";
    }

    for (int i = 0; i <(int)TeamSetting::MaxPawnTeam; i++) {
        mPawnTeamTable[i] = new PawnTeam("images/entity/team/null_team.png", i);
        mPawnTeamTable[i]->mName = "null_pawn_team";
    }

    TextureManager tm;
    mTempTex = tm.CreateTempTexture(System::sRenderer,
        System::sWindowWidth + 4000, System::sWindowHeight + 4000
    );
}

//아직 로직 더 필요함.
void TeamManager::AllocTeamOnTable(std::string name, std::string path, int id)
{
    Team* team = mTeamTable[id];

    team->mName = name;

    team->mTeamTex->LoadFromFile(path);
}

void TeamManager::DeallocTeamOnTable(int id)
{
    mTeamTable[id]->mName = "null_team";
}

void TeamManager::AllocPTeamOnTable(std::string name, int id)
{
    mPawnTeamTable[id]->mName = name;
}

void TeamManager::DeallocPTeamOnTable(int id)
{
    mPawnTeamTable[id]->mName = "null_pawn_team";
}

void TeamManager::RenderOnUpdate(Map* map)
{
    if (mIsTeamUpdate == false) {
        SDL_RenderTexture(System::sRenderer , mTempTex, &map->mCam->mSight, nullptr);
        return;
    }

    SDL_Log("teammanager render update");

    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

    //실제 렌더링 로직, 아직 미구현
    for (Team* team : mTeamTable) {
        if (!team->isOnMap) continue; //맵에 존재하는지 플래그

        team->mTeamTex->Render(team->mMapPosX, team->mMapPosY);
    }

    for (PawnTeam* pTeam : mPawnTeamTable) {
        if (!pTeam->isOnMap) continue;

        pTeam->mTeamTex->Render(pTeam->mMapPosX, pTeam->mMapPosY);
    }

    //렌더러 타겟 해제
    SDL_SetRenderTarget(System::sRenderer, NULL);
    //렌더링 완료하면 플래그 변수 초기화
    mIsTeamUpdate = false;
}

void TeamManager::SpawnTeamOnMap(Map *map, Team *team, int id)
{
    team->isOnMap = true;

    //아이디로 좌표를 구한다.
    MapHelper mh;
    std::unordered_map<std::string, int> xy;
    xy = mh.PosXYByTileId(id, map);

    team->mMapPosX = map->mOffsetX + map->mTileLen * xy["x"];
    team->mMapPosY = map->mOffsetY + map->mTileLen * xy["y"];
    team->mId = id;

    MapTile* tile = map->mMapTiles[id];
}

void TeamManager::SpawnTeamOnMap(Map *map, Team *team, int x, int y)
{
    team->isOnMap = true;
    team->mMapPosX = x; team->mMapPosY = y;

    //타일 id 구함
    MapHelper mh;
    int id = mh.WhatTileOnPoint(x, y, map);
    //타일 아이디 팀에 할당
    team->mTileId = id;

    //타일에 데이터 로드
    //타일 객체 구함
    MapTile* tile = map->mMapTiles[id];
}

void TeamManager::PutEntInTeam(Team* team, Entity* ent)
{
    //현재 멤버 수가 최대일때
    if (team->mTeamMates.size() == team->mMaxMember) {
        SDL_Log("cannot put entity into team: hitting team member limit");
        return;
    }
    //매개변수 엔티티의 아이디가 팀메이트 엔티티들의 아이디 중 하나와 겹치는 경우
    for (auto teamMate : team->mTeamMates) {
        if (teamMate.first == ent->mId) {
            SDL_Log("cannot put entity into team: entity already in team");
            return;
        } 
    }
    //멤버가 최대가 아닌 경우
    team->mTeamMates.insert({ent->mId, ent});

    std::string message = "current teammates: " + std::to_string(team->mTeamMates.size());
    SDL_Log(message.c_str());
}

void TeamManager::OutEntInTeam(Team *team, int id)
{
    //매개변수 아이디에 해당되는 엔티티를 찾지 못했을 경우
    if (team->mTeamMates.end() == team->mTeamMates.find(id)) {
        SDL_Log("cannot out entity from team: no corresponding entity in team");
        return;
    }
    //팀에서 엔티티 제외
    team->mTeamMates.erase(id);

    std::string message = "current teammates: " + std::to_string(team->mTeamMates.size());
    SDL_Log(message.c_str());
}

EntityManager::EntityManager(GameContext* gc)
{
    for (int i = 0; i < (int)EntitySetting::MaxEnt; i++) {
        mEntTable[i] = new Entity("null_entity", i);
    }

    for (int i = 0; i < (int)EntitySetting::MaxPawn; i++) {
        mPawnTable[i] = new Pawn(*gc->mObjm, "null_pawn", PawnType::Null, i);
    }

    mEntAI = new AI(gc);

    TextureManager tm;
}

void EntityManager::AllocEntityOnTable(ObjectManager &objm, std::string name, int subMapX, int subMapY, int id)
{
    const json& entItems = objm.mJsm->mEntDb["items"]; //데이터베이스 가져오기

    json entData;

    //엔티티 코드 확인
    if (entItems.contains(name)) {
        //json에 이름이 포함된 경우
        entData = entItems[name];
    }
    else {
        SDL_Log("entity name not found");
        name = "error_entity";
        entData = entItems[name];
    }

    Entity* ent = mEntTable[id];

    ent->mMapX = subMapX;
    ent->mMapY = subMapY;

    //텍스처 할당
    ent->mTexture->LoadFromFile(entData["img_path"].get<std::string>());

    //데이터 읽고 가져오기
    ent->mName = entData["name"].get<std::string>();
    json entSkills = entData["skills"];
    for (json skill : entSkills) {
        std::string skillcode = skill.get<std::string>();
        ent->mSkills.push_back(new Skill(skillcode, skillcode));
    }
    
    ent->mStr = entData["str"].get<int>();
    ent->mEnd = entData["end"].get<int>();
    ent->mDex = entData["dex"].get<int>();
    ent->mAgi = entData["agi"].get<int>();
    ent->mWil = entData["wil"].get<int>();
    ent->mSpd = entData["spd"].get<int>();

    //최대체력, 최대행동력은 스탯에 따라 결정
    StatHelper sh;
    ent->mCurHp = sh.GetMaxHp(ent);
    ent->mCurSp = sh.GetMaxSp(ent);
    ent->mCurAp = sh.GetMaxAp(ent);

    //선천적으로 방어력을 가진 경우 패시브 플래그에서 가져오는 걸루..

    std::string message = "entity id: " + std::to_string(id) + " name: " + name + " is allocated";
    SDL_Log(message.c_str());
}

void EntityManager::AllocPawnOnTable(ObjectManager &objm, std::string name, PawnType pType, int id)
{
    const json& pawnItems = objm.mJsm->mPawnDb["items"];

    if (!pawnItems.contains(name)) {
        name = "error_pawn";
    }
    json pawnData = pawnItems[name];

    Pawn* pawn = mPawnTable[id];


    pawn->mName = pawnData["name"].get<std::string>();
    pawn->mCustomName = pawn->mName;
    pawn->mType = pType;
    pawn->mDemeanor = Demeanor::Friendly;

    pawn->mHead.mCount = 1;
    pawn->mTorso.mCount = 1;
    pawn->mLower.mCount = 1;
    pawn->mHand.mCount = 2;
    pawn->mFoot.mCount = 2;

    StatHelper sh;
    pawn->mCurHp = sh.GetMaxHp(pawn);
    pawn->mCurSp = sh.GetMaxSp(pawn); 
    pawn->mCurAp = sh.GetMaxAp(pawn); 

    pawn->mIsPawn = true;
    
    //텍스처 할당
    pawn->mTexture->LoadFromFile(pawnData["img_path"].get<std::string>());

    //TODO:스킬 배우는 동작도 엔티티 생성자에서 언젠가 분리시켜야 한다.
    pawn->mSkills.push_back(new Skill("move", "이동"));
    pawn->mSkills.push_back(new Skill("punch", "주먹질"));
    pawn->mSkills.push_back(new Skill("pickup", "줍기"));

    std::string message = "pawn id: " + std::to_string(id) + " name: " + name + " is allocated";
    SDL_Log(message.c_str());
}

void EntityManager::DeallocEntityOnTable(ObjectManager& objm, int id)
{
    AllocEntityOnTable(objm, "null_entity", -1, -1, id);
    SDL_Log("deallocated entity");
}

void EntityManager::DeallocPawnOnTable(ObjectManager &objm, int id)
{
    AllocPawnOnTable(objm, "null_pawn", PawnType::Null, id);
    SDL_Log("deallocated pawn");
}

void EntityManager::KillEntityOnMap(GameContext& gc, Map* map, Entity* ent)
{
    gc.mSkm->mTargets.clear();
    if (ent->mIsOnMap) DespawnEntity(*gc.mObjm, map, ent);

    if (ent->mIsPawn) {
        DeallocPawnOnTable(*gc.mObjm, ent->mId);
    }
    else {
        DeallocEntityOnTable(*gc.mObjm, ent->mId);
    }
}

void EntityManager::SpawnEntityOnMap(ObjectManager &objm, Map *map, Entity *ent)
{

}

void EntityManager::SpawnEntityOnMap(ObjectManager &objm, Map *map, Entity *ent, int tileId)
{
    ent->mIsOnMap = true;
    MapTile* tile = map->mMapTiles[tileId]; 
    tile->mIsEntOn = true;

    ent->mTileId = tileId;

    std::unordered_map<std::string, int> xy;
    MapHelper mh;
    xy = mh.PosXYByTileId(tileId, map);

    ent->mMapX = map->mOffsetX + map->mTileLen * xy["x"];
    ent->mMapY = map->mOffsetY + map->mTileLen * xy["y"];
    std::string message = "entity spawned at: " + std::to_string(ent->mMapX) + ", " + std::to_string(ent->mMapY);
    SDL_Log(message.c_str());

    //맵 엔티티 컨테이너에 엔티티를 추가한다.
    if (ent->mIsPawn) map->mPawns.push_back(ent);
    else map->mNpcs.push_back(ent);
}

//엔티티 스폰의 정확히 반대 역할을 한다.
void EntityManager::DespawnEntity(ObjectManager &objm, Map *map, Entity *ent)
{
    ent->mIsOnMap = false;
    int tid = ent->mTileId;
    MapTile* tile = map->mMapTiles[tid];
    tile->mIsEntOn = false;
    if (ent->mIsPawn) {
        int i = 0;
        for (Entity* p : map->mPawns) {
            if (p->mId == ent->mId) {
                map->mPawns.erase(map->mPawns.begin() + i);
                break;
            }
            i++;
        }
    }
    else {
        int i = 0;
        for (Entity* npc : map->mNpcs) {
            if (npc->mId == ent->mId) {
                map->mNpcs.erase(map->mNpcs.begin() + i);
                break;
            }
            i ++;
        }
    }
}

bool EntityManager::PickUpItemFromMap(GameContext& gc, int tileId, int itemId, Pawn *p)
{
    bool itemPicked = false;

    Item* target = gc.mObjm->mItm->PopSpecificItem(gc.mMapm->mCurrentMap, tileId, itemId);
    LogUI* log = gc.mUim->mLogUI;

    if (target == nullptr) {
        SDL_Log("pickup item: item was nullptr!");
        return itemPicked;
    }

    PickUpItem(gc, p, target);
    
    std::string message = "picked item id: " + std::to_string(itemId);
    SDL_Log(message.c_str());
    itemPicked = true;
    return itemPicked;
}

bool EntityManager::PickUpItem(GameContext& gc, Pawn *p, Item *item)
{
    bool success = false;

    p->mInventory[item->mId] = item;
    
    LogUI* log = gc.mUim->mLogUI;
    if ((int) p->mInventory.size() == System::sInvCap) {
        SDL_Log("pick up item: inventory full!");
        log->AddMessage("인벤토리가 가득 찼습니다!", System::kY);
        return false;
    }
    
    item->mTileId = (int) p->mInventory.size() - 1;
    success = true;
    return success;
}

bool EntityManager::EraseFromInv(Pawn *p, int itemId)
{
    bool success = false;

    Item* item = p->mInventory[itemId]; //아이템 캐싱
    //실제 삭제
    p->mInventory.erase(itemId);

    //아이템 타일 위치 조정(이렇게 해서 정렬함)
    for (auto itemPair : p->mInventory) {
        //타일 id가 타겟 아이템보다 큰 객체들을 대상으로 타일 id를 하나씩 감소시킨다.
        if (itemPair.second->mTileId > item->mTileId) {
            itemPair.second->mTileId -= 1;
        }
    }
    success = true;
    return success;
}

bool EntityManager::EquipItem(GameContext &gc, Pawn* p, Equipment* eq)
{
    bool success = false;

    //내가 장비한 장비들 중 같은 장비 타입인 것들을 순회한다.
    for (auto range = p->mEqs.equal_range(eq->mEqType); range.first != range.second;
    range.first++) 
    {
        Equipment* targetEq = range.first->second;
        //조건을 검색해 이미 장비하고 있는 무언가가 있다면 continue 한다.
        if (targetEq != nullptr) continue;
        //장비 슬롯이 비어있을 경우
        range.first->second = eq; //장비한게 없다면 mEq 맵에 매개변수 장비를 할당해준다.
        EraseFromInv(p, eq->mId); //아이템을 인벤토리에서 삭제한다.
        break;
    }

    //장비 슬롯 객체의 정보도 업데이트한다.
    CharacterSheetUI* cui = gc.mUim->mCharacterSheet;
    for (auto range = cui->mInvUI->mSlotInfos.equal_range(eq->mEqType);
        range.first != range.second; range.first++) 
    {
        SlotInfo& si = range.first->second;
        if (si.mEqId != 0) continue; //슬롯의 장비 아이디가 0이면 장비가 없음. 반대의 경우 continue
        si.mEqId = eq->mId;
        cui->mInvUI->LoadEqToolTip(si);
        success = true;
        break;
    }

    SDL_Log("item equipped");
    cui->mIsRenderUpdate = true;
    return success;
}

bool EntityManager::UnequipItem(GameContext &gc, Pawn *p, int itemId)
{
    bool success = false;

    EntityUtil eu;
    Equipment* eq = nullptr;
    
    //매개변수 아이디와 일치하는 장비를 찾는다.
    for (auto iter = p->mEqs.begin(); iter != p->mEqs.end(); iter++) {
        if (iter->second == nullptr) continue;
        if (iter->second->mId == itemId) {
            eq = iter->second;
            
            iter->second = nullptr;
            break;
        }
    }
    //만약 장비를 찾지 못했다면 즉시 반환.
    if (eq == nullptr) {
        SDL_Log("unequip item: cannot find item id param from equipments!");
        return false;
    }
    //슬롯 참조 데이터도 초기화한다.
    InventoryUI* inv = gc.mUim->mCharacterSheet->mInvUI;
    SlotInfo si;
    for (auto iter = inv->mSlotInfos.begin(); iter != inv->mSlotInfos.end(); iter++) {
        if (iter->second.mEqId == itemId) {
            iter->second.mEqId = 0;
            si = iter->second;
            SDL_Log(std::to_string(iter->second.mEqId).c_str());
            break;
        }
    }

    //인벤토리에 다시 넣는다.
    PickUpItem(gc, p, eq);
    CharacterSheetUI* cui = gc.mUim->mCharacterSheet;
    SDL_Log("item unequipped");
    cui->mInvUI->LoadEqToolTip(si);
    cui->mIsRenderUpdate = true;
    success = true;
    return success;
}

void EntityManager::Update(ObjectManager &objm)
{
}

void EntityManager::HandleEvent(SDL_Event &e, GameContext& gc, Map* map, float mouseX, float mouseY)
{   
    if (!mCanHandleEvent) return;
    //카메라 오프셋 계산
    mouseX += map->mCam->mSight.x;
    mouseY += map->mCam->mSight.y;

    //각 엔티티에 대해 이벤트 핸들링
    for (Entity* p : map->mPawns) {
        HandleEntityEvent(e, gc, map, p, mouseX, mouseY);
    }
    for (Entity* npc : map->mNpcs) {
        HandleEntityEvent(e, gc, map, npc, mouseX, mouseY);
    }
}

void EntityManager::HandleEntityEvent(SDL_Event &e, GameContext &gc, Map *map, Entity *ent, float mx, float my)
{
    //엔티티 이벤트 핸들러는 포커스 상태만 제어함
    if (!System::sIsLeftMouseClicked || e.type != SDL_EVENT_MOUSE_BUTTON_UP) return; //마우스가 올라갔을때 감지

    MapTile* tile = map->mMapTiles[ent->mTileId]; //타일 기준으로 이벤트 핸들링
    Math ph;
    bool mouseIn = ph.IsPointInSquare(mx, my, tile->mX, tile->mY, tile->mW, tile->mH);
    if (!mouseIn) return;

    SkillHelper skh;
    //스킬이 준비되었다면 클릭된 엔티티는 타겟이 된다.
    if (gc.mSkm->mIsSkillReady) {
        json sd = gc.mSkm->mSkillData;
        Skill* sk = gc.mSkm->mSkill;
        int tn = skh.GetSkillTargetNum(sd, sk);

        if (gc.mSkm->mTargets.size() > tn) {
            //최대 타겟 개수를 넘어가면 리턴함.
            return; 
        }

        gc.mSkm->SetTarget(ent);
        SDL_Log("skill target entity set");
        return;
    } 
    
    FocusEntity(gc, map, ent);
}

void EntityManager::FocusEntity(GameContext &gc, Map *map, Entity *ent)
{
    std::string message = "focusing on " + ent->mName;
    SDL_Log(message.c_str());
    
    //엔티티 태도에 따라 포커스 텍스처를 변경.
    gc.mUim->mFocusIcon->SetDimension(ent->mMapX, ent->mMapY, map->mTileLen, map->mTileLen);
    if (ent->mDemeanor == Demeanor::Neutral) gc.mUim->mFocusIcon->mTex->LoadFromFile("images/ui/focus.png");
    else if (ent->mDemeanor == Demeanor::Friendly) gc.mUim->mFocusIcon->mTex->LoadFromFile("images/ui/focus_friendly.png");
    else if (ent->mDemeanor == Demeanor::Hostile) gc.mUim->mFocusIcon->mTex->LoadFromFile("images/ui/focus_hostile.png");
    gc.mUim->mFocusIcon->mIsRender = true;

    gc.mUim->mTileHLUI->ClearTileIds();
    gc.mUim->mBCUI->UpdateUI(ent);

    //포커스 엔티티 캐싱
    mFocusedEnt = ent;
    //아군이 아닐 경우 타일 범위 렌더링 끔
    if (!ent->mIsPawn) gc.mUim->mTileHLUI->mIsRenderBetweenTiles = false;
    else mFocusedPc = static_cast<Pawn*>(ent);
    gc.mUim->mQSUI->Activate();
    
    //이전 엔티티와 같은 경우
    if (ent == mPrevFocusedEnt)  {
        //포커스된 엔티티를 한번 더 클릭했을 경우 
        mPrevFocusedEnt = ent;
        if (!ent->mIsPawn) return; //아군이 아니면 반환함. 
        SDL_Log("one more click on focused pawn");
        Pawn* p = static_cast<Pawn*>(ent);
        //스킬 ui등을 표시.
        gc.mUim->mCharacterSheet->Activate();
        gc.mEvCtx->mIsEventHandled = true;
    }

    mPrevFocusedEnt = ent;
}

void EntityManager::RenderEntities(Map *map)
{
    for (Entity* ent : mEntTable) {
        if (!ent->mIsOnMap) continue;
        ent->mTexture->Render(ent->mMapX - map->mCam->mSight.x, ent->mMapY - map->mCam->mSight.y,
            nullptr, (float) map->mTileLen, (float) map->mTileLen
        );
 
    }
    for (Pawn* pawn : mPawnTable) {
        if (!pawn->mIsOnMap) continue;
        pawn->mTexture->Render(pawn->mMapX - map->mCam->mSight.x, pawn->mMapY - map->mCam->mSight.y,
            nullptr, (float) map->mTileLen, (float) map->mTileLen
        );
    }
}

Entity::Entity(std::string name, int id)
{
    mId = id;

    mTexture = new Texture();
}

//부하 생성자
Pawn::Pawn(const ObjectManager& objm, std::string name, PawnType pType, int id)
{  
    mId = id;
    mName = name;

    mTexture = new Texture();
    
    //지금은 하드코딩 했는데 나중에는 인간 디폴트 데이터를 불러오게 할 수도 있다.
    mStr = 10;
    mEnd = 10;
    mPer = 10;
    mDex = 10;
    mAgi = 10;
    mWil = 10;
    mInt = 10;
    mSpd = 10;

    mEqs.insert({EqType::Head, nullptr});
    mEqs.insert({EqType::Back, nullptr});
    mEqs.insert({EqType::Torso, nullptr});
    mEqs.insert({EqType::Leg, nullptr});
    mEqs.insert({EqType::Hand, nullptr});
    mEqs.insert({EqType::Hand, nullptr});
    mEqs.insert({EqType::Foot, nullptr});
    mEqs.insert({EqType::Foot, nullptr});
    mEqs.insert({EqType::Weapon, nullptr});
    mEqs.insert({EqType::Weapon, nullptr});
}

int StatHelper::GetMaxHp(Entity *ent)
{
    int ret = ent->mEnd * 7 + ent->mWil * 3;
    return ret;
}

int StatHelper::GetMaxSp(Entity *ent)
{
    int ret = ent->mEnd * 7 + ent->mStr * 3;
    return ret;
}

int StatHelper::GetMaxAp(Entity *ent)
{
    int ret = ent->mSpd * 10;
    return ret;
}

float StatHelper::GetTotalWeight(Entity *ent)
{
    float weight = 0;
    //장비의 무게 합산
    for (auto eq : ent->mEqs) {
        if (eq.second == nullptr) continue;
        weight += eq.second->mWeight;
    }
    //TODO:
    //종족값에 따라 무게 추가
    //인벤토리 아이템에 따라 무게 추가
    //패시브, 임시 효과에 따라 무게 추가

    return weight;
}

int StatHelper::GetMediumWeightLimit(Entity *ent)
{
    return ent->mStr * 5;
}

int StatHelper::GetMaxWeightLimit(Entity *ent)
{
    return ent->mStr * 10;
}

int StatHelper::GetApPerTileMove(Entity *ent)
{
    //페널티가 없는 무게의 하한일 경우
    if (ent->mWeight < (float) GetMediumWeightLimit(ent)) {
        return 50;
    }
    //최대 무게의 하한일 경우
    else if (ent->mWeight < (float) GetMaxWeightLimit(ent)) {
        return 100;
    }
    //중량 초과
    else {
        return 9999;
    }
}

int StatHelper::GetTotalArmor(Entity *ent)
{
    //장비의 방어력 합산
    int armor = 0;
    for (auto eq : ent->mEqs) {
        if (eq.second == nullptr) continue;
        armor += eq.second->mArmor;
    }
    //TODO:
    //패시브 플래그에 따라 방어력 추가
    //임시 효과에 따라 방어력 추가
    //종족값에 따라 방어력 추가

    return armor;
}

//아직 덜만듬
int StatHelper::GetHpRegen(Entity *ent)
{
    return 0;
}

//아직 덜만듬
int StatHelper::GetSpRegen(Entity *ent)
{
    return 50;
}

//턴 지나면 최대로 회복
int StatHelper::GetApRegen(Entity *ent)
{
    int ret = GetMaxAp(ent);
    return ret;
}

SDL_Color EntityUtil::GetDemeanorColor(Entity *ent)
{
    if (ent->mDemeanor == Demeanor::Friendly) return {0x40, 0xB0, 0x40, 0xFF};
    else if (ent->mDemeanor == Demeanor::Hostile) return {0xB0, 0x40, 0x40, 0xFF};
    else if (ent->mDemeanor == Demeanor::Neutral) return {0xB0, 0xB0, 0x40, 0xFF};
    else {
        SDL_Log("entity util: cannot find demeanor type of entity");
        return {0xFF, 0xFF, 0xFF, 0xFF};
    }
}

SDL_Color EntityUtil::GetRDemeanorColor(Entity *ent)
{
    if (ent->mDemeanor == Demeanor::Friendly) return {0xB0, 0x40, 0x40, 0xFF};
    else if (ent->mDemeanor == Demeanor::Hostile) return {0x40, 0xB0, 0x40, 0xFF};
    else if (ent->mDemeanor == Demeanor::Neutral) return {0xB0, 0xB0, 0x40, 0xFF};
    else {
        SDL_Log("entity util: cannot find demeanor type of entity");
        return {0xFF, 0xFF, 0xFF, 0xFF};
    }
}

Equipment *EntityUtil::GetEquipment(Pawn *p, int itemId)
{
    Equipment* eq = nullptr;
    
    for (auto eqPair : p->mEqs) {
        if (eqPair.second == nullptr) continue;
        if (eqPair.second->mId == itemId) {
            eq = eqPair.second;
            break;
        }
    }

    return eq;
}
