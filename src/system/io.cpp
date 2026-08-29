#include "pch.h"

#include "system/io.h"

#include "game_context.h"
#include "game_object.h"
#include "system/game_json.h"
#include "entity.h"
#include "item/item.h"
#include "skill/skill.h"
#include "map.h"

using ojson = nlohmann::ordered_json;

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

void GameIO::Load(GameContext *gc, string name)
{
    CharacterIO::LoadPawn(gc, name);
}

void CharacterIO::SavePawn(GameContext *gc, string name)
{
    using json = nlohmann::json;
    ofstream out;

    out.open("save/userdata/" + name + "/pawn.json");

    ojson j;
    j["type"] = "pawn_save";
    j["items"];

    
    for (Pawn* p : gc->mObjm->mEntm->mPawnTable) {
        if (p->mName == "null_pawn") continue;
        j["items"][p->mName] = {
            {"name", p->mName},
            {"code", p->mCode},
            {"type", p->mType},
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

            {"skill", GetSkillData(p)},
            {"quick_skill", GetQSkillData(p)},
            {"equipments", GetEqquipedEqData(p)},
            {"inventory", GetInvData(p)}
        };
    }

    out << j.dump(4);
    out.close();
}

void CharacterIO::LoadPawn(GameContext *gc, string name)
{
    json save;
    JsonHelper::LoadJsonFile(save, "save/userdata/" + name + "/pawn.json");

    for (json pdata : save["items"]) {
        gc->mObjm->mEntm->AllocPawnOnTable(
            gc, pdata["code"].get<string>(), pdata["type"].get<PawnType>(),
            pdata["id"].get<int>()
        );

        Pawn* p = gc->mObjm->mEntm->mPawnTable[pdata["id"].get<int>()];

        // gc->mObjm->mEntm->mFocusedEnt = p;
        gc->mObjm->mEntm->mFocusedPc = p;

        p->mIsOnMap = pdata["is_on_map"].get<bool>();
        p->mTileId = pdata["tile_id"].get<int>();
        p->mMapX = pdata["map_x"].get<int>();
        p->mMapY = pdata["map_y"].get<int>();

        p->mStr = pdata["str"].get<int>();
        p->mEnd = pdata["end"].get<int>();
        p->mDex = pdata["dex"].get<int>();
        p->mPer = pdata["per"].get<int>();
        p->mAgi = pdata["agi"].get<int>();
        p->mWil = pdata["wil"].get<int>();
        p->mInt = pdata["int"].get<int>();
        p->mSpd = pdata["spd"].get<int>();

        p->mCurHp = pdata["cur_hp"].get<int>();
        p->mCurSp = pdata["cur_sp"].get<int>();
        p->mCurAp = pdata["cur_ap"].get<int>();
    
        p->mWeight = pdata["weight"].get<float>();

        EntityManager* entm = gc->mObjm->mEntm;
        for (ojson eqdata : pdata["equipments"]) {
            entm->EquipItem(*gc, p, ItemIO::LoadEquipment(gc, eqdata));
        }

        for (ojson itemdata : pdata["inventory"]) {
            Item* item {nullptr};
            if (itemdata["type"].get<ItemType>() == ItemType::Equipment) {
                item = ItemIO::LoadEquipment(gc, itemdata);
            }
            else if (itemdata["type"].get<ItemType>() == ItemType::Consumable) {
                item = ItemIO::LoadConsumable(gc, itemdata);
            }
            
            entm->PickUpItem(*gc, p, item);
        }

        if (p->mIsOnMap) gc->mObjm->mEntm->SpawnEntityOnMap(*gc->mObjm, gc->mMapm->mCurrentMap, p);
    }

}

void CharacterIO::SaveNpc(GameContext *gc, string name)
{
    using json = nlohmann::json;
    ofstream out;

    out.open("save/userdata/" + name + "/npc.json");

    ojson j;
    j["type"] = "pawn_save";
    j["items"];

    
}

void CharacterIO::LoadNpc(GameContext *gc, string name)
{
    json save;
    JsonHelper::LoadJsonFile(save, "save/userdata/" + name + "/npc.json");

}

ojson CharacterIO::GetSkillData(Pawn* p)
{
    ojson ret;

    for(Skill* sk : p->mSkills) {
        ret += sk->mCode;
    }

    return ret;
}

ojson CharacterIO::GetQSkillData(Pawn *p)
{
    json ret;

    for (string skillcode : p->mQuickSkills) {
        ret += skillcode;
    }

    return ret;
}


ojson CharacterIO::GetItemData(Item *item)
{
    if (item->mType == ItemType::Equipment) {
        return GetEqData((Equipment*) item);
    }
    else if (item->mType == ItemType::Consumable) {
        return GetConsumableData((Consumable*) item);
    }
    else {
        SDL_Log("[WARNING] io/get item data: unknown item type");
        return ojson::object();
    }
}

void CharacterIO::AddCommonItemData(Item *item, ojson &oj)
{
    oj += {"code", item->mCode};
    oj += {"name", item->mName};
    oj += {"type", item->mType};
    oj += {"id", item->mId};
    oj += {"tile_id", item->mTileId};
    oj += {"value", item->mValue};
    oj += {"weight", item->mWeight};
}

ojson CharacterIO::GetEqData(Equipment *eq)
{
    ojson ret = nlohmann::ordered_json::object();

    AddCommonItemData(eq, ret);

    ret += {"eq_type", eq->mEqType};
    ret += {"damage", eq->mDamage};
    ret += {"armor", eq->mArmor};

    ret += {"str_flat", eq->mStrFlat};
    ret += {"agi_flat", eq->mAgiFlat};
    ret += {"dex_flat", eq->mDexFlat};
    ret += {"per_flat", eq->mPerFlat};
    ret += {"end_flat", eq->mEndFlat};
    ret += {"wil_flat", eq->mWilFlat};
    ret += {"int_flat", eq->mIntFlat};
    ret += {"spd_flat", eq->mSpdFlat};

    return ret;
}

ojson CharacterIO::GetEqquipedEqData(Pawn *p)
{
    ojson ret;

    for (auto eqPair : p->mEqs) {
        if (eqPair.second == nullptr) continue;
        Equipment* eq = eqPair.second;
        ret += GetEqData(eq);
    }

    return ret;
}

ojson CharacterIO::GetInvData(Pawn *p)
{
    ojson ret;

    for (auto itemPair : p->mInventory) {
        ret += GetItemData(itemPair.second);
    }

    return ret;
}

ojson CharacterIO::GetConsumableData(Consumable *cons)
{
    ojson ret = ojson::object();

    AddCommonItemData(cons, ret);

    return ret;
}

void GameIO::MakeUserSave(string name)
{
    std::filesystem::create_directories("save/userdata/" + name);
}

void ItemIO::LoadCommonItemData(json itemdata, Item *item)
{
    using namespace std;
    item->mCode = itemdata["code"].get<string>();
    item->mName = itemdata["name"].get<string>();
    item->mValue = itemdata["value"].get<int>();
    item->mWeight = itemdata["weight"].get<float>();
    item->mTileId = itemdata["tile_id"].get<int>();
}

Equipment *ItemIO::LoadEquipment(GameContext* gctx, json eqdata)
{
    using namespace std;
    Equipment* eq = new Equipment(*gctx->mObjm,
         eqdata["code"].get<string>(), eqdata["eq_type"].get<EqType>()
    );

    eq->mTileId = eqdata["tile_id"].get<int>();
    eq->mEqType = eqdata["eq_type"].get<EqType>();
    eq->mDamage = eqdata["damage"].get<int>();
    eq->mArmor = eqdata["armor"].get<int>();

    eq->mStrFlat = eqdata["str_flat"].get<int>();
    eq->mAgiFlat = eqdata["agi_flat"].get<int>();
    eq->mDexFlat = eqdata["dex_flat"].get<int>();
    eq->mPerFlat = eqdata["per_flat"].get<int>();
    eq->mEndFlat = eqdata["end_flat"].get<int>();
    eq->mWilFlat = eqdata["wil_flat"].get<int>();
    eq->mIntFlat = eqdata["int_flat"].get<int>();
    eq->mSpdFlat = eqdata["spd_flat"].get<int>();

    return eq;
}

Consumable *ItemIO::LoadConsumable(GameContext *gctx, json consdata)
{
    using namespace std;
    Consumable* cons = new Consumable(*gctx->mObjm, consdata["code"].get<string>());

    return cons;
}
