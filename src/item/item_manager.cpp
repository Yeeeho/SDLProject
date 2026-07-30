#include "pch.h"

#include "entity.h"
#include "item/item_manager.h"
#include "game_json.h"
#include "item/item.h"
#include "item/item_manager.h"

ItemManager::ItemManager()
{
    LoadItemData();
}

void ItemManager::LoadItemData()
{
    JsonHelper jh;
    jh.LoadJsonFile(mItemCodeTable, "data/item/item_code_table.json");
    jh.LoadJsonFile(mWeaponDb, "data/item/weapon.json");
    jh.LoadJsonFile(mGearDb, "data/item/gear.json");
    jh.LoadJsonFile(mUWeaponDb, "data/item/unique_weapon.json");
    jh.LoadJsonFile(mUGearDb, "data/item/unique_gear.json");
    jh.LoadJsonFile(mUseItDb, "data/item/useable.json");
    jh.LoadJsonFile(mSpecItDb, "data/item/special.json");
}

EqType ItemHelper::GetEqType(json eq)
{
    std::string type = eq["type"].get<std::string>();

    if (type == "weapon") return EqType::Weapon;
    else if (type == "offhand") return EqType::Offhand;
    else if (type == "head") return EqType::Head;
    else if (type == "torso") return EqType::Torso;
    else if (type == "leg") return EqType::Leg;
    else if (type == "hand") return EqType::Hand;
    else if (type == "foot") return EqType::Foot;
    else if (type == "all") return EqType::All;
    else if (type == "back") return EqType::Back;
    else return EqType::All;
}

int ItemHelper::GetEqDamage(json eq)
{
    if (eq.contains("base_dmg")) {
        int ret = eq["base_dmg"].get<int>();

        return ret;
    }
    else return 0;
}

int ItemHelper::GetEqDamage(json eq, Entity *ent)
{
    int dmg = GetEqDamage(eq); //기본 공격력을 우선 구한다이

    if (eq.contains("dmg_scale")) {
        json scale = eq["dmg_scale"];
        if (scale.contains("str")) {
           float strMod = scale["str"].get<float>();
           dmg += (int) (ent->mStr * strMod);
        }
        if (scale.contains("end")) {
            float endMod = scale["end"].get<float>();
            dmg += (int) (ent->mEnd * endMod);
        }
        if (scale.contains("dex")) {
            float mod = scale["dex"].get<float>();
            dmg += (int) (ent->mDex * mod);
        }
        if (scale.contains("agi")) {
            float mod = scale["agi"].get<float>();
            dmg += (int) (ent->mAgi * mod);
        }
        if (scale.contains("wil")) {
            float mod = scale["wil"].get<float>();
            dmg += (int) (ent->mWil * mod);
        }
        if (scale.contains("int")) {
            float mod = scale["int"].get<float>();
            dmg += (int) (ent->mInt * mod);
        }
        if (scale.contains("spd")) {
            float mod = scale["spd"].get<float>();
            dmg += (int) (ent->mSpd * mod);
        }
    }

    return dmg;
}

int ItemHelper::GetEqArmor(json eq) {
    if (eq.contains("base_armor")) return eq["base_armor"].get<int>();
    else return 0;
}

json* ItemHelper::GetEqDb(ItemManager *itm, std::string code)
{
    json items = itm->mItemCodeTable["items"];
    if (items.contains(code)) {
        std::string type = items[code].get<std::string>();
        if (type == "weapon") return &itm->mWeaponDb;
        else if (type == "unique_weapon") return &itm->mUWeaponDb;
        else if (type == "gear") return &itm->mGearDb;
        else if (type == "unique_gear") return &itm->mUGearDb;
        else if (type == "useable") return &itm->mUseItDb;
        else if (type == "special") return &itm->mSpecItDb;
        else {
            SDL_Log("type name error in item code table");
            return &itm->mItemCodeTable; //여기에 에러 틀어막기용 json 객체를..
        }
    }
    else {
        SDL_Log("cannot find key(code) in item code table");
        return &itm->mItemCodeTable;
    }
}

float ItemHelper::GetEqWeight(json eq) {
    if (eq.contains("base_weight")) return eq["base_weight"].get<float>();
    else return 0.f;
}