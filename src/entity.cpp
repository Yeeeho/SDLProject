#include "pch.h"

#include "entity.h"
#include "math.h"
#include "render.h"
#include "system.h"
#include "ui.h"
#include "game_object.h"
#include "map.h"
#include "camera.h"
#include "text.h"
#include "game_json.h"
#include "texture.h"
#include "item.h"
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
    MapManager mm;
    std::unordered_map<std::string, int> xy;
    xy = mm.PosXYByTileId(id, map);

    team->mMapPosX = map->mX + map->mTileLen * xy["x"];
    team->mMapPosY = map->mY + map->mTileLen * xy["y"];
    team->mId = id;

    MapTile* tile = map->mMapTiles[id];
    LoadDataInTile(tile, team);
}

void TeamManager::SpawnTeamOnMap(Map *map, Team *team, int x, int y)
{
    team->isOnMap = true;
    team->mMapPosX = x; team->mMapPosY = y;

    //타일 id 구함
    MapManager mm;
    int id = mm.WhatTileOnPoint(x, y, map);
    //타일 아이디 팀에 할당
    team->mTileId = id;

    //타일에 데이터 로드
    //타일 객체 구함
    MapTile* tile = map->mMapTiles[id];
    LoadDataInTile(tile, team);
}

void TeamManager::LoadDataInTile(MapTile* tile, Team *team)
{
    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
    TTFWord* teamName = new TTFWord(team->mName, tc, System::sFont);

    tile->mInfos.push_back(teamName);
    tile->mInfos.push_back(new TTFWord(System::sFont, TextType::NewLine));
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

EntityManager::EntityManager(ObjectManager& objm)
{
    for (int i = 0; i < (int)EntitySetting::MaxEnt; i++) {
        mEntTable[i] = new Entity("null_entity", i);
    }

    for (int i = 0; i < (int)EntitySetting::MaxPawn; i++) {
        mPawnTable[i] = new Pawn(objm, "null_pawn", PawnType::Null, i);
    }

    TextureManager tm;
}

void EntityManager::AllocEntityOnTable(ObjectManager &objm, std::string name, int subMapX, int subMapY, int id)
{
    const json& entItems = objm.mJsm->mEntDb["items"]; //데이터베이스 가져오기

    json entData;

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
    ent->mName = entData["name"].get<std::string>(); //문자열은 24바이트를 처먹기 때문에 고쳐야 할지도
    
    ent->mStr = entData["str"].get<int>();
    ent->mEnd = entData["end"].get<int>();
    ent->mDex = entData["dex"].get<int>();
    ent->mAgi = entData["agi"].get<int>();
    ent->mWil = entData["wil"].get<int>();
    ent->mSpd = entData["spd"].get<int>();

    //최대체력, 최대행동력은 스탯에 따라 결정
    StatHelper sh;
    ent->mCurHp = sh.GetMaxHp(ent);
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

    StatHelper sh;
    pawn->mCurHp = sh.GetMaxHp(pawn); 
    pawn->mCurAp = sh.GetMaxAp(pawn); 

    pawn->mIsPawn = true;
    
    //텍스처 할당
    pawn->mTexture->LoadFromFile(pawnData["img_path"].get<std::string>());

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

void EntityManager::LoadDataInTile(MapTile *tile, Entity *ent)
{
    tile->mIsEntOn = true;

    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
    SDL_Color yellow = {0xB0, 0xB0, 0x40, 0xFF};
    SDL_Color white = {0xF0, 0xF0, 0xF0, 0xFF};
    SDL_Color red = {0xB0, 0x40, 0x40, 0xFF};
    SDL_Color blue = {0x40, 0x40, 0xB0, 0xFF};
    if (ent->mDemeanor == Demeanor::Hostile) tc = {0xB0, 0x00, 0x00, 0xFF};
    if (ent->mDemeanor == Demeanor::Neutral) tc = yellow;

    TTFWord* name = new TTFWord(ent->mName, tc, System::sFont);

    StatHelper sh;

    tile->mInfos.push_back(name);
    tile->mInfos.push_back(new TTFWord(System::sFont, TextType::NewLine));

    tile->mInfos.push_back(new TTFWord("HP:", red, System::sFont));
    tile->mInfos.push_back(new TTFWord(System::sFont, TextType::Space));
    tile->mInfos.push_back(new TTFWord(std::to_string(ent->mCurHp), white, System::sFont));
    std::string maxHp = "/" + std::to_string(sh.GetMaxHp(ent));
    tile->mInfos.push_back(new TTFWord(maxHp, white, System::sFont));
    tile->mInfos.push_back(new TTFWord(System::sFont, TextType::NewLine));

    tile->mInfos.push_back(new TTFWord("AP:", blue, System::sFont));
    tile->mInfos.push_back(new TTFWord(System::sFont, TextType::Space));
    tile->mInfos.push_back(new TTFWord(std::to_string(ent->mCurAp), white, System::sFont));
    std::string maxAp = "/" + std::to_string(sh.GetMaxAp(ent));
    tile->mInfos.push_back(new TTFWord(maxAp, white, System::sFont));
    tile->mInfos.push_back(new TTFWord(System::sFont, TextType::NewLine));
}

void EntityManager::SpawnEntityOnMap(ObjectManager &objm, Map *map, Entity *ent)
{

}

void EntityManager::SpawnEntityOnMap(ObjectManager &objm, Map *map, Entity *ent, int tileId)
{
    ent->mIsOnMap = true;

    ent->mTileId = tileId;

    std::unordered_map<std::string, int> xy;
    MapManager mm;
    xy = mm.PosXYByTileId(tileId, map);

    ent->mMapX = map->mX + map->mTileLen * xy["x"];
    ent->mMapY = map->mY + map->mTileLen * xy["y"];
    std::string message = "entity spawned at: " + std::to_string(ent->mMapX) + ", " + std::to_string(ent->mMapY);
    SDL_Log(message.c_str());

    //타일에 데이터를 로드해준다.
    MapTile* tile = map->mMapTiles[tileId]; 
    LoadDataInTile(tile, ent);
    //맵 엔티티 컨테이너에 엔티티를 추가한다.
    if (ent->mIsPawn) map->mPawns.push_back(ent);
    else map->mNpcs.push_back(ent);
}

void EntityManager::Update(ObjectManager &objm)
{
}

void EntityManager::HandleEvent(SDL_Event &e, UIManager& uim, ObjectManager& objm, Map* map, float mouseX, float mouseY)
{
    //오른쪽 마우스 버튼 클릭시 포커스 해제
    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN && e.button.button == SDL_BUTTON_RIGHT) {
        map->mFocusedEnt = nullptr; map->mPrevFocusedEnt = nullptr;
        uim.mFocusIcon->mIsRender = false;
        return;
    } 
    //카메라 오프셋 계산
    mouseX += map->mCam->mSight.x;
    mouseY += map->mCam->mSight.y;

    for (Entity* p : map->mPawns) {
        p->HandleEvent(e, uim, objm, map, mouseX, mouseY);
    }
    for (Entity* npc : map->mNpcs) {
        npc->HandleEvent(e, uim, objm, map, mouseX, mouseY);
    }
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

void Entity::HandleEvent(SDL_Event &e, UIManager &uim, ObjectManager &objm, Map* map,  float x, float y)
{
    //엔티티 이벤트 핸들러는 포커스 상태만 제어함
    if (e.type != SDL_EVENT_MOUSE_BUTTON_DOWN) return; //마우스 왼쪽 클릭
    if (e.button.button != SDL_BUTTON_LEFT) return;

    MapTile* tile = map->mMapTiles[mTileId]; //타일 기준으로 이벤트 핸들링
    Math ph;
    bool mouseIn = ph.IsPointInSquare(x, y, tile->mX, tile->mY, tile->mW, tile->mH);
    if (!mouseIn) return;
    
    //엔티티 태도에 따라 포커스 텍스처를 변경.
    uim.mFocusIcon->SetDimension(mMapX, mMapY, map->mTileLen, map->mTileLen);
    if (mDemeanor == Demeanor::Neutral) uim.mFocusIcon->mTex->LoadFromFile("images/ui/focus.png");
    else if (mDemeanor == Demeanor::Friendly) uim.mFocusIcon->mTex->LoadFromFile("images/ui/focus_friendly.png");
    else if (mDemeanor == Demeanor::Hostile) uim.mFocusIcon->mTex->LoadFromFile("images/ui/focus_hostile.png");
    uim.mFocusIcon->mIsRender = true;

    //포커스 엔티티 캐싱
    map->mFocusedEnt = this;
    //아군이 아닐 경우 타일 범위 렌더링 끔
    if (!this->mIsPawn) uim.mTileHLUI->mIsRenderBetweenTiles = false;

    //이전 엔티티와 같은 경우
    if (this == map->mPrevFocusedEnt)  {
        //포커스된 엔티티를 한번 더 클릭했을 경우 
        SDL_Log("one more click on focused entity");
        map->mPrevFocusedEnt = this;
        if (!mIsPawn) return; //아군이 아니면 반환함. 

        //스킬 ui등을 표시.
    }

    map->mPrevFocusedEnt = this;
    //내가 아군이고 턴을 가지고 있을때
    
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
    mDex = 10;
    mAgi = 10;
    mWil = 10;
    mInt = 10;
    mSpd = 10;

    mEqs[EqType::Head] = new Equipment(objm, "naked");
    mEqs[EqType::Torso] = new Equipment(objm, "naked");
    mEqs[EqType::Leg] = new Equipment(objm, "naked");
    mEqs[EqType::Hand] = new Equipment(objm, "naked");
    mEqs[EqType::Foot] = new Equipment(objm, "naked");
    mEqs[EqType::Weapon] = new Equipment(objm, "naked");
    mEqs[EqType::Offhand] = new Equipment(objm, "naked");
}

int StatHelper::GetMaxHp(Entity *ent)
{
    int ret = ent->mEnd * 10 + ent->mWil * 5;
    return ret;
}

int StatHelper::GetMaxAp(Entity *ent)
{
    int ret = ent->mSpd * 10;
    return ret;
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
