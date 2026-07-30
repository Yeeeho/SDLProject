#pragma once

#include "item_enum.h"

using json = nlohmann::json;

class Entity;

class ItemManager {
    public:
    ItemManager();

    void LoadItemData();

    json mItemCodeTable;

    json mWeaponDb;
    json mGearDb;
    json mUWeaponDb;
    json mUGearDb;
    json mUseItDb;
    json mSpecItDb;
};

class ItemHelper {
    public:
    EqType GetEqType(json eq);
    float GetEqWeight(json eq); 

    int GetEqDamage(json eq);
    int GetEqDamage(json eq, Entity* ent);
    int GetEqArmor(json eq);

    json* GetEqDb(ItemManager* itm, std::string code);
};