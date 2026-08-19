#include "pch.h"

#include "math.h"
#include "camera.h"
#include "game_context.h"
#include "game_json.h"
#include "system.h"
#include "render.h"
#include "map.h"
#include "texture.h"
#include "text.h"
#include "entity.h"
#include "item/item_manager.h"
#include "item/item.h"
#include "ui.h"
#include "skill/skill.h"

ItemManager::ItemManager(GameContext* gc)
{
    mGc = gc;
    LoadSpriteSheets();
    LoadItemData();

    for (int i = 0; i < kMaxItemId; i++) {
        mIdTable[i] = i;
    }

    mConsumableSs = new Texture("images/item/consumable/consumable_ss.png");

    TextureManager tm;
    mTempTex = tm.CreateTempTexture();
}

//공변 반환
Item *ItemManager::MakeItem(ObjectManager& objm, std::string code, ItemType it)
{
    if (it == ItemType::Consumable) {
        return new Consumable(objm, code);
    }
    else {
        SDL_Log("item manager: cannot make item from code");
        return new Item();
    }
}

Equipment *ItemManager::MakeEq(ObjectManager& objm, std::string code, EqType et)
{
    return new Equipment(objm, code, et);
}

void ItemManager::LoadSpriteSheets()
{
    mConsumableSs = new Texture("images/item/consumable/consumable_ss.png");
    mWeaponSs = new Texture("images/item/weapon/weapon_ss.png");
    mGearSs = new Texture("images/item/gear/gear_ss.png");
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
    jh.LoadJsonFile(mGearSsMap, "images/item/gear/gear_ss.json");

    jh.LoadJsonFile(mUseItDb, "data/item/useable.json");
    jh.LoadJsonFile(mConsumItDb, "data/item/consumable.json");
    jh.LoadJsonFile(mConsumItSsMap, "images/item/consumable/consumable_ss.json");

    jh.LoadJsonFile(mSpecItDb, "data/item/special.json");
}

void ItemManager::SpawnItemOnMap(Map *map, int tileId, Item *item)
{
    item->mTileId = tileId;
    StackItemOnMap(map, tileId, item);

    std::string message = "item spawned at tild id: "
        + std::to_string(tileId) + " item code: " + item->mCode
        + " item id: " + std::to_string(item->mId); 
    SDL_Log(message.c_str());    

    //타일에 데이터를 로드
    mGc->mUim->mToolTip->mIsRenderUpdate = true;
    mIsRenderUpdate = true;
}

void ItemManager::DespawnItemOnMap(Map * map, int tileId, int itemId)
{
    if (map->mItemStackMap.find(tileId) == map->mItemStackMap.end()) {
        //스택 자체가 이미 없을 경우
        SDL_Log("despawn item: stack does not exist on map!");
        return;
    }
    //스택이 있는 경우
    ItemStack* stack = map->mItemStackMap[tileId];
    int i = 0;
    for (Item* item : stack->mStack) {
        if (item->mId == itemId) {
            stack->mStack.erase(stack->mStack.begin() + i);
            mIsRenderUpdate = true;
            return;
        }
        i++;
    }    
}

void ItemManager::HandleEvent(SDL_Event &e, GameContext *gc, float mx, float my)
{
    if (!mCanHandleEvent) return;
    Map* map = gc->mMapm->mCurrentMap;

    //마우스 오프셋
    mx += map->mCam->mSight.x;
    my += map->mCam->mSight.y;

    MapHelper mh;
    int tileId = mh.WhatTileOnPoint(mx, my, map);

    //타일에 스택이 생성되지 않았다면 리턴함
    if (map->mItemStackMap.find(tileId) == map->mItemStackMap.end()) return;
    //타일에 스택이 있을 경우

    Item* item = map->mItemStackMap[tileId]->mStack.back();
    HandleItemEvent(e, gc, item, mx, my);
}

void ItemManager::HandleItemEvent(SDL_Event& e, GameContext* gc, Item* item, float mx, float my) 
{
    if (e.type != SDL_EVENT_MOUSE_BUTTON_DOWN || e.button.button != SDL_BUTTON_LEFT) return;
    
    MapTile* tile = gc->mMapm->mCurrentMap->mMapTiles[item->mTileId];

    Math mth;
    bool isIn = mth.IsPointInSquare(mx, my, (float) tile->mX, (float) tile->mY, (float) tile->mW, (float) tile->mH);
    if (!isIn) return;
    //아이템이 있는 타일에 마우스가 들어왔을 경우

    //스킬이 준비된 경우 아이템을 타겟으로 설정함.
    if (gc->mSkm->mIsSkillReady) {
        gc->mSkm->SetTargetItem(item);
    }
}

int ItemManager::GetValidId()
{
    for (int i = 0; i < kMaxItemId; i++) {
        if (mIdTable[i] == 0) continue;
        int ret = mIdTable[i];
        mIdTable[i] = 0;
        return ret;
    }

    SDL_Log("ran out of item index!");
    return -1;
}

void ItemManager::ReturnId(int id)
{
    mIdTable[id] += id;
}

void ItemManager::StackItemOnMap(Map *map, int tileId, Item *item)
{
    if (map->mItemStackMap.find(tileId) == map->mItemStackMap.end()) {
        //아이템을 스택에 없을 경우
        map->mItemStackMap[tileId] = new ItemStack();
        map->mItemStackMap[tileId]->mStack.push_back(item);
    }
    else {
        //아이템이 스택에 이미 있는 경우
        map->mItemStackMap[tileId]->mStack.push_back(item);
    }
}

//리턴할 아이템을 찾지 못하면 널포인터를 반환한다.
Item *ItemManager::PopSpecificItem(Map *map, int tileId, int itemId)
{
    if (map->mItemStackMap.find(tileId) == map->mItemStackMap.end()) {
        SDL_Log("pop specific item from stack: cannot find item stack object, param: tileId");
        return nullptr;
    }

    ItemHelper ih;
    ItemStack* stackObj = map->mItemStackMap[tileId];
    if (stackObj->mStack.empty()) {
        ih.DestroyStackObj(map, tileId);
        SDL_Log("pop specific item from stack: stack already empty");
        return nullptr;
    }

    int i = 0;
    Item* targetItem = nullptr;
    //스택 순회하며 아이템 검색
    for (Item* item : stackObj->mStack) {
        if (item->mId == itemId) {
            //아이템 찾으면 스택에서 참조만 지움
            stackObj->mStack.erase(stackObj->mStack.begin() + i);
            targetItem = item;
            break;
        }
        i++;
    }

    //아이템을 못 찾았을 경우 널포인터 반환
    if (targetItem == nullptr) {
        SDL_Log("pop specific item from stack: specific item not found in stack");
        return targetItem;
    }
    //아이템을 찾았을 경우
    //만약 스택이 비었다면 스택 오브젝트 파괴
    if (stackObj->mStack.empty()) {
        ih.DestroyStackObj(map, tileId);
    }

    return targetItem;
}

Item* ItemManager::PopItemStack(Map *map, int tileId)
{
    ItemHelper ih;
    ItemStack* stackObj = map->mItemStackMap[tileId];

    //비었으면 해제해준다.
    if (stackObj->mStack.empty()) {
        ih.DestroyStackObj(map, tileId);
        SDL_Log("pop item from stack: stack already empty");
        return nullptr;
    }

    Item* item = map->mItemStackMap[tileId]->mStack.back();

    map->mItemStackMap[tileId]->mStack.pop_back();

    if (stackObj->mStack.empty()) {
        ih.DestroyStackObj(map, tileId);
    }

    return item;
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

    Map* currentMap = mGc->mMapm->mCurrentMap;

    for (auto stack : currentMap->mItemStackMap) {
        Item* item = stack.second->mStack.back();
        RenderItem(item);
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
    float itemX = (float) (xy["x"] * currentMap->mTileLen + currentMap->mOffsetX);
    float itemY = (float) (xy["y"] * currentMap->mTileLen + currentMap->mOffsetY);

    RenderItem(item, itemX, itemY, currentMap->mTileLen, currentMap->mTileLen);
    SDL_Log("rendered an item");
}

void ItemManager::RenderItem(Item* item, float x, float y, float w, float h) 
{
    ItemHelper ih;

    json* db = ih.GetItemDb(this, item);
    json* ssmap = ih.GetItemSsMap(this, item);

    json itemData = (*db)["items"][item->mCode];
    std::string sname = itemData["sprite_name"].get<std::string>();

    json sprites = (*ssmap)["sprites"];
    for (json sp : sprites) {
        if (sp["filename"].get<std::string>() == sname) {
            Texture* ss = ih.GetItemSs(this, item);
            SDL_FRect fr = {sp["x"].get<float>(), sp["y"].get<float>(),
                sp["width"].get<float>(), sp["height"].get<float>() 
            };
            ss->Render(x, y, &fr, (float) w, (float) h);
        }
    }
}


void ItemManager::Render()
{
    RenderStoredTex();
    StoreTexture();
}

void ItemHelper::DestroyStackObj(Map* map, int tileId) {
    ItemStack* stackObj = map->mItemStackMap[tileId];        
    map->mItemStackMap.erase(tileId);
    delete stackObj;
    SDL_Log("deleted stack object");
}

EqType ItemHelper::GetEqType(json eqData)
{
    std::string type = eqData["sub_type"].get<std::string>();

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
    else if (item->mType == ItemType::Equipment) {
        Equipment* eq = static_cast<Equipment*> (item);
        if (eq->mEqType == EqType::Weapon) return &itm->mWeaponSsMap;
        else return &itm->mGearSsMap;
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
    else if (item->mType == ItemType::Equipment) {
        Equipment* eq = static_cast<Equipment*>(item);
        if (eq->mEqType == EqType::Weapon) return itm->mWeaponSs;
        else return itm->mGearSs;
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