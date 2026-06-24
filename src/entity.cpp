#include "pch.h"

#include "texture.h"
#include "entity.h"
#include "item.h"

using json = nlohmann::json;

Team::Team(std::string path)
{
    

    mTeamTex = new Texture();
    if (mTeamTex->LoadFromFile(path) == false) {
        std::string message = path + " not loaded";
        SDL_Log(message.c_str());
    }
}

Entity::Entity(std::string name)
{
    SDL_Log("this is abstract entity");
}

//부하 생성자
Pawn::Pawn(const ObjectManager& objm, std::string name, PawnType pType)
{  

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

Enemy::Enemy(const ObjectManager& objm, std::string name, PawnType pType)
{
}
