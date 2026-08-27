#include "pch.h"

#include "system/io.h"

#include "game_context.h"
#include "system/game_json.h"
#include "game_object.h"
#include "entity.h"
#include "item/item.h"
#include "skill/skill.h"
#include "map.h"

void IOHelper::MakeDir(string path)
{
    ofstream out;
    out.open(path);

    out.close();
}

std::string IOHelper::Indent(int num)
{
    string ret = "";
    for (int i = 0; i < num; i++) {
        ret += " ";
    }

    return ret;
}

std::string IOHelper::Wrap(std::string message)
{
    return "\"" + message + "\"";
}

void GameIO::Save(GameContext *gc, string name)
{
    MakeUserSave(name);

    ofstream out;

    CharacterIO::SavePawn(gc, name);
}

void CharacterIO::SavePawn(GameContext *gc, string name)
{
    using json = nlohmann::json;
    using ojson = nlohmann::ordered_json;
    ofstream out;

    out.open("save/userdata/" + name + "/characters.json");

    ojson j;
    j["type"] = "characters_save";
    j["items"];

    
    for (Pawn* p : gc->mObjm->mEntm->mPawnTable) {
        j["items"][p->mName] = {
            {"name", p->mName},
            {"id", p->mId},
            {"is_on_map", p->mIsOnMap},
            {"tile_id", p->mTileId},
            {"map_x", p->mMapX},
            {"map_y", p->mMapY},

            {"str", p->mStr},
            {"end", p->mEnd},
            {"dex", p->mDex},
            {"per", p->mPer},
            {"agi", p->mAgi},
            {"wil", p->mWil},
            {"int", p->mInt},
            {"spd", p->mSpd},

            {"cur_hp", p->mCurHp},
            {"cur_sp", p->mCurSp},
            {"cur_ap", p->mCurAp},

            {"weight", p->mWeight},

            {"skill", GetSkillData(p)}
        };
    }

    out << j.dump(4);
    out.close();
}

json CharacterIO::GetSkillData(Pawn* p)
{
    json ret;

    for(Skill* sk : p->mSkills) {
        ret += sk->mCode;
    }

    return ret;
}

void GameIO::MakeUserSave(string name)
{
    std::filesystem::create_directories("save/userdata/" + name);
}

