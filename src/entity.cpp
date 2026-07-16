#include "pch.h"

#include "entity.h"
#include "render.h"
#include "system.h"
#include "game_object.h"
#include "map.h"
#include "camera.h"
#include "text.h"
#include "game_json.h"
#include "texture.h"
#include "item.h"

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

    SDL_Log("updating teams on update flag");

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
    mTempTex = tm.CreateTempTexture(System::sRenderer,
        System::sWindowWidth + 4000, System::sWindowHeight + 4000
    );
    SDL_SetTextureScaleMode(mTempTex, SDL_SCALEMODE_NEAREST);
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

    ent->mSubMapX = subMapX;
    ent->mSubMapY = subMapY;

    ent->mName = entData["name"].get<std::string>();
    ent->mRace = entData["race"].get<std::string>();
    //텍스처 할당
    ent->mTexture->LoadFromFile(entData["img_path"].get<std::string>());
    
    if (entData.contains("hp")) ent->mMaxHp = entData["hp"].get<int>();
    ent->mCurHp = ent->mMaxHp;

    if (entData.contains("ap")) ent->mMaxAp = entData["ap"].get<int>();
    ent->mCurAp = ent->mMaxAp;

    if (entData.contains("spd")) ent->mMaxSpd = entData["spd"].get<int>();
    ent->mCurSpd = ent->mMaxSpd;

    if (entData.contains("atk")) ent->mMaxAtk = entData["atk"].get<int>();
    ent->mCurAtk = ent->mMaxAtk;

    if (entData.contains("armor")) ent->mMaxArmor = entData["armor"].get<int>();
    ent->mCurArmor = ent->mMaxArmor; 

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

    //텍스처 할당
    pawn->mTexture->LoadFromFile(pawnData["img_path"].get<std::string>());

    pawn->mType = pType;

    pawn->mName = pawnData["name"].get<std::string>();
    pawn->mOriginalName = pawn->mName;
    //하드코딩
    pawn->mRace = "human";

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
    SDL_Color tc = {0x00, 0xB0, 0x00, 0xFF};
    TTFWord* name = new TTFWord(ent->mName, tc, System::sFont);

    tile->mInfos.push_back(name);
    tile->mInfos.push_back(new TTFWord(System::sFont, TextType::NewLine));

    tc = {0xB0, 0xB0, 0xB0, 0xFF};
    tile->mInfos.push_back(new TTFWord("클릭하여 자세한 정보를 보기(미구현)", tc, System::sFont));
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

    ent->mSubMapX = map->mX + map->mTileLen * xy["x"];
    ent->mSubMapY = map->mY + map->mTileLen * xy["y"];

    MapTile* tile = map->mMapTiles[tileId];

    LoadDataInTile(tile, ent);
}

void EntityManager::RenderOnUpdate(Map* map)
{
    if (mIsRenderUpdate == false) {
        SDL_RenderTexture(System::sRenderer, mTempTex, &map->mCam->mSight, nullptr);
        return;
    }

    SDL_Log("render update");
    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

    for (Entity* ent : mEntTable) {
        if (!ent->mIsOnMap) continue;
        ent->mTexture->Render(ent->mSubMapX, ent->mSubMapY, nullptr,
           (float) map->mTileLen, (float) map->mTileLen
        );
 
    }
    for (Pawn* pawn : mPawnTable) {
        if (!pawn->mIsOnMap) continue;
        pawn->mTexture->Render(pawn->mSubMapX, pawn->mSubMapY, nullptr,
           (float) map->mTileLen, (float) map->mTileLen
        );
    }

    //렌더러 타겟 해제
    SDL_SetRenderTarget(System::sRenderer, nullptr);
    mIsRenderUpdate = false;
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

    mMaxHp = 100;
    mMaxAp = 100;
    mMaxSpd = 100;
    mMaxAtk = 10;
    mMaxArmor = 10;

    mEqs[EqType::Head] = new Equipment(objm, "naked");
    mEqs[EqType::Torso] = new Equipment(objm, "naked");
    mEqs[EqType::Leg] = new Equipment(objm, "naked");
    mEqs[EqType::Hand] = new Equipment(objm, "naked");
    mEqs[EqType::Foot] = new Equipment(objm, "naked");
    mEqs[EqType::Weapon] = new Equipment(objm, "naked");
    mEqs[EqType::Offhand] = new Equipment(objm, "naked");

    //유니크 부하를 생성한다.
    if (pType == PawnType::Unique) {
        //아직 미구현
    }
}
