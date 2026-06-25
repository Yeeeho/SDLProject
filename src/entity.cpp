#include "pch.h"

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
        mTeamTable[i] = new Team("images/entity/team.png" , i);
    }

    for (int i = 0; i <(int)TeamSetting::MaxPawnTeam; i++) {
        mPawnTeamTable[i] = new PawnTeam("images/entity/team.png", i);
    }

    TextureManager tm;
    mTempTex = tm.CreateTempTexture();
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
