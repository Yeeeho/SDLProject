#include "pch.h"

#include "system.h"
#include "game_object.h"
#include "game_json.h"
#include "texture.h"
#include "entity.h"
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
    mTempTex = tm.CreateTempTexture();
}

//아직 로직 더 필요함.
void TeamManager::AllocTeamOnTable(std::string name, int id)
{
    mTeamTable[id]->mName = name;
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

void TeamManager::RenderOnUpdate()
{
    if (mIsTeamUpdate == false) {
        SDL_RenderTexture(System::sRenderer , mTempTex, nullptr, nullptr);
        return;
    }

    SDL_Log("updating teams on update flag");
    SDL_SetRenderTarget(System::sRenderer, mTempTex); //렌더러 타겟으로 설정
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_SetTextureBlendMode(mTempTex, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
    SDL_RenderClear(System::sRenderer);

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

void TeamManager::SpawnTeamOnMap(Team *team, int x, int y)
{
    team->isOnMap = true;
    team->mMapPosX = x; team->mMapPosY = y;
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
}

EntityManager::EntityManager(ObjectManager& objm)
{
    for (int i = 0; i < (int)EntitySetting::MaxEnt; i++) {
        mEntTable[i] = new Entity("null_entity", i);
    }

    for (int i = 0; i < (int)EntitySetting::MaxPawn; i++) {
        mPawnTable[i] = new Pawn(objm, "null_pawn", PawnType::Null, i);
    }
}

void EntityManager::AllocEntityOnTable(ObjectManager &objm, std::string name,  int id)
{
    const json& entItems = objm.mJsm->mEntDb["items"]; //데이터베이스 가져오기

    json entData;

    if (entItems.contains(name)) {
        //json에 이름이 포함된 경우
        entData = entItems[name];
    }
    else {
        name = "error_entity";
        entData = entItems[name];
    }

    Entity* ent = mEntTable[id];

    ent->mName = name;

    ent->mRace = entData["race"].get<std::string>();

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
}

void EntityManager::AllocPawnOnTable(ObjectManager &objm, std::string name, PawnType pType, int id)
{
    Pawn* pawn = mPawnTable[id];
    
    pawn->mType = pType;

    pawn->mName = name;
    //하드코딩
    pawn->mRace = "human";
}

void EntityManager::DeallocEntityOnTable(ObjectManager& objm, int id)
{
    for (Entity* ent : mEntTable) {
        if (ent->mId == id) {
            AllocEntityOnTable(objm, "null_entity", id);
            return;
        }
    }
}

void EntityManager::DeallocPawnOnTable(ObjectManager &objm, int id)
{
    for (Pawn* pawn : mPawnTable) {
        if (pawn->mId == id) {
            AllocPawnOnTable(objm, "null_pawn", PawnType::Null, id);
            return;
        }
    }
}

Entity::Entity(std::string name, int id)
{
    mId = id;
}

//부하 생성자
Pawn::Pawn(const ObjectManager& objm, std::string name, PawnType pType, int id)
{  
    mId = id;
    mName = name;

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
