#include "pch.h"

#include "camera.h"
#include "game_context.h"
#include "game_json.h"
#include "system.h"
#include "render.h"
#include "map.h"
#include "texture.h"
#include "entity.h"
#include "item/item_manager.h"
#include "item/item.h"

ItemManager::ItemManager(GameContext* gc)
{
    mGc = gc;
    LoadSpriteSheets();
    LoadItemData();

    for (int i = 0; i <  1024; i++) {
        mIdTable[i] = i;
    }

    mConsumableSs = new Texture("images/item/consumable/consumable_ss.png");

    TextureManager tm;
    mTempTex = tm.CreateTempTexture();
}

//공변 반환
Item *ItemManager::MakeItem(ObjectManager& objm, std::string code, ItemType it)
{
    if (it == ItemType::Equipment) {
        return new Equipment(objm, code);
    }
    else if (it == ItemType::Consumable) {
        return new Consumable(objm, code);
    }
    else {
        SDL_Log("item manager: cannot make item from code");
        return new Item();
    }
}

void ItemManager::LoadSpriteSheets()
{
    mConsumableSs = new Texture("images/item/consumable/consumable_ss.png");
}

void ItemManager::LoadItemData()
{
    JsonHelper jh;
    jh.LoadJsonFile(mItemCodeTable, "data/item/item_code_table.json");
    jh.LoadJsonFile(mWeaponDb, "data/item/weapon.json");
    jh.LoadJsonFile(mUWeaponDb, "data/item/unique_weapon.json");
    jh.LoadJsonFile(mWeaponSsMap, "images/item/weapon/weapon_ss.json");

    jh.LoadJsonFile(mGearDb, "data/item/gear.json");
    jh.LoadJsonFile(mUGearDb, "data/item/unique_gear.json");
    
    jh.LoadJsonFile(mUseItDb, "data/item/useable.json");
    jh.LoadJsonFile(mConsumItDb, "data/item/consumable.json");
    jh.LoadJsonFile(mConsumItSsMap, "images/item/consumable/consumable_ss.json");

    jh.LoadJsonFile(mSpecItDb, "data/item/special.json");
}

void ItemManager::SpawnItemOnMap(Item *item, std::map<int, Item*>& mapItems, int tileId)
{
    item->mTileId = tileId;
    std::string message = "item spawned at tild id " + std::to_string(tileId); 
    SDL_Log(message.c_str());
    mapItems.insert({item->mId, item});
    mIsRenderUpdate = true;
}

void ItemManager::SpawnItemOnMap(Item *item, std::map<int, Item*>& mapItems, Map* map, int xpos, int ypos)
{
    MapHelper mh;
    int tileId = mh.WhatTileOnPoint((float) xpos, (float) ypos, map);

    SpawnItemOnMap(item, mapItems, tileId);
}

void ItemManager::DespawnItemOnMap(int itemId, std::map<int, Item*>& mapItems)
{
    mapItems.erase(itemId);
}

int ItemManager::GetValidId()
{
    for (int i : mIdTable) {
        if (i == 0) continue;
        else return i;
    }

    SDL_Log("ran out of item index!");
    return -1;
}

void ItemManager::ReturnId(int id)
{
    mIdTable[id] += id;
}

void ItemManager::StoreTexture()
{
    if (!mIsRenderUpdate) return;
    SDL_Log("item manager: storing texture");
    RenderManager rm;
    rm.SetRenderTarget(System::sRenderer, mTempTex);

    RenderItems();

    SDL_SetRenderTarget(System::sRenderer, nullptr);
    mIsRenderUpdate = false;
    SDL_Log("item manager: stored texture");
}

void ItemManager::RenderStoredTex()
{
    if (!mIsRender) return;
    Camera* cam = mGc->mMapm->mCurrentMap->mCam;
    SDL_FRect fr = {-cam->mSight.x, -cam->mSight.y, cam->mSight.w, cam->mSight.h};
    SDL_RenderTexture(System::sRenderer, mTempTex, nullptr, &fr);
}

void ItemManager::RenderItems()
{
    ItemHelper ih;

    for (auto pair : mSubmapItems) {
        RenderItem(pair.second);
    }
    for (auto pair : mCitymapItems) {
        RenderItem(pair.second);
    }
}

void ItemManager::RenderItem(Item* item)
{
    SDL_Log("rendering an item");
    SDL_Log(std::to_string((int) item->mType).c_str());
    ItemHelper ih;
    MapHelper mm;
    Map* currentMap = mGc->mMapm->mCurrentMap;
    std::unordered_map<std::string, int> xy = mm.PosXYByTileId(item->mTileId, mGc->mMapm->mCurrentMap);
    //TODO: 이런것도 래핑해야 좌표 구하기 편해질거다.
    float itemX = (float) (xy["x"] * currentMap->mTileLen + currentMap->mX);
    float itemY = (float) (xy["y"] * currentMap->mTileLen + currentMap->mY);

    json* db = ih.GetItemDb(this, item);

    json itemData = (*db)["items"][item->mCode];
    std::string sname = itemData["sprite_name"].get<std::string>();

    json* map = ih.GetItemSsMap(this, item);
    json sprites = (*map)["sprites"];
    for (json sp : sprites) {
        if (sp["filename"].get<std::string>() == sname) {
            Texture* ss = ih.GetItemSs(this, item);
            SDL_FRect fr = {sp["x"].get<float>(), sp["y"].get<float>(),
                sp["width"].get<float>(), sp["height"].get<float>() 
            };
            ss->Render(itemX, itemY, &fr, (float) currentMap->mTileLen, (float) currentMap->mTileLen);
        }
    }
}

void ItemManager::Render()
{
    RenderStoredTex();
    StoreTexture();
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

json *ItemHelper::GetItemDb(ItemManager *itm, json scenarioData)
{
    std::string db = scenarioData["item_type"].get<std::string>();

    if (db == "consumable") return &itm->mConsumItDb;
    else if (db == "special") return &itm->mSpecItDb;
    else{
        SDL_Log("get item db: cannot find db in scenario data");
        return &itm->mSpecItDb;
    } 
}

json *ItemHelper::GetItemSsMap(ItemManager *itm, Item *item)
{
    if (item->mType == ItemType::Consumable)  {
        return &itm->mConsumItSsMap;
    }
    else {
        SDL_Log("get item sprite sheet map: item type unknown!");
        return &itm->mConsumItSsMap;
    }
}

Texture *ItemHelper::GetItemSs(ItemManager *itm, Item *item)
{
    if (item->mType == ItemType::Consumable) {
        return itm->mConsumableSs;
    }
    else {
        SDL_Log("get item sprite sheet: item type unknown!");
        return itm->mConsumableSs;
    }
}

json *ItemHelper::GetItemDb(ItemManager *itm, Item *item)
{
    if (item->mType == ItemType::Consumable) {
        return &itm->mConsumItDb;
    }
    else if (item->mType == ItemType::Equipment) {
        Equipment* eq = static_cast<Equipment*> (item);
        return GetEqDb(itm, eq);
    }
    else {
        SDL_Log("get item db: item type unknown!");
        std::string message = "item type was: " + std::to_string((int) item->mType);
        SDL_Log(message.c_str());
        return &itm->mSpecItDb;
    }
}

json *ItemHelper::GetEqDb(ItemManager *itm, Equipment *eq)
{
    if (eq->mEqType == EqType::All) return &itm->mSpecItDb;
    if (eq->mEqType == EqType::Error) return &itm->mSpecItDb;

    if (eq->mEqType == EqType::Head) return &itm->mGearDb;
    if (eq->mEqType == EqType::Torso) return &itm->mGearDb;
    if (eq->mEqType == EqType::Leg) return &itm->mGearDb;
    if (eq->mEqType == EqType::Hand) return &itm->mGearDb;
    if (eq->mEqType == EqType::Foot) return &itm->mGearDb;
    if (eq->mEqType == EqType::Back) return &itm->mGearDb;

    if (eq->mEqType == EqType::Weapon) return &itm->mWeaponDb;
    if (eq->mEqType == EqType::Offhand) return &itm->mWeaponDb;

    SDL_Log("get equipment db: equipment type unknown!");
    return &itm->mSpecItDb;
}

ItemType ItemHelper::GetItemType(std::string iType)
{
    if (iType == "consumable") return ItemType::Consumable;
    else if (iType == "artifact") return ItemType::Artifact;
    else if (iType == "equipment") return ItemType::Equipment;
    else {
        SDL_Log("get item type: cannot interpret string into enum item type!");
        return ItemType::Error;
    }
}

float ItemHelper::GetEqWeight(json eq) {
    if (eq.contains("base_weight")) return eq["base_weight"].get<float>();
    else return 0.f;
}