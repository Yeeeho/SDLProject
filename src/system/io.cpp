#include "pch.h"

#include "system/io.h"

#include "game_context.h"
#include "game_object.h"
#include "system/game_json.h"
#include "entity.h"
#include "item/item_manager.h"
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

std::string IOHelper::GetMapTypeName(Map *map)
{
    string ret = "";
    if(map->mMapType == MapType::OverMap) ret = "overmap";
    else if(map->mMapType == MapType::SubMap) ret = "submap";
    if(map->mMapType == MapType::CityMap) ret = "citymap";

    return ret;
}

void GameIO::Save(GameContext *gc, string name)
{
    MakeUserSave(name);

    ofstream out;

    CharacterIO::SavePawn(gc, name);
    CharacterIO::SaveNpc(gc, name);
    MapIO::SaveMaps(gc, name);
}

void GameIO::Load(GameContext *gc, string name)
{
    CharacterIO::LoadPawn(gc, name);
    CharacterIO::LoadNpc(gc, name);
    MapIO::LoadMaps(gc, name);
}


void MapIO::SaveMap(GameContext *gctx, Map *map, ojson& mapdata)
{
    SaveMapEntities(map, mapdata);
    SaveMapItems(map, mapdata);
}

void MapIO::LoadMaps(GameContext *gctx, string name)
{
    json savedata;
    JsonHelper::LoadJsonFile(savedata, "save/userdata/" + name + "/map.json");

    LoadMap(gctx, gctx->mMapm->mSubMap, savedata["items"]["submap"]);
    LoadMap(gctx, gctx->mMapm->mOverMap, savedata["items"]["overmap"]);
    LoadMap(gctx, gctx->mMapm->mCityMap, savedata["items"]["citymap"]);
}

void MapIO::LoadMap(GameContext *gctx, Map *map, json &mapdata)
{
    EntityManager* entm = gctx->mObjm->mEntm;
    for (ojson npcdata : mapdata["npcs"]) {
        int npcId = npcdata.get<int>();
        //이미 엔티티 테이블을 로드한 상태이므로 테이블을 조회하기만 하고 객체를 생성하지는 않는다.
        entm->SpawnEntityOnMap(*gctx->mObjm, map, entm->mEntTable[npcId]);
    }
    for (ojson pawndata : mapdata["pawns"]) {
        int pawnId = pawndata.get<int>();
        entm->SpawnEntityOnMap(*gctx->mObjm, map, entm->mPawnTable[pawnId]);
    }   
    //아이템의 경우 테이블이 따로 존재하지 않으므로 로드까지 진행한다.
    for (ojson itemdata : mapdata["items"]) {
        Item* item = ItemIO::LoadItem(gctx, itemdata, nullptr);
        if (item == nullptr) {
            SDL_Log("null");
            continue;
        }
        gctx->mObjm->mItm->SpawnItemOnMap(map, item->mTileId, item);
    }
}

void MapIO::SaveMaps(GameContext *gctx, string name)
{
    using json = nlohmann::json;
    ofstream out;

    out.open("save/userdata/" + name + "/map.json");

    ojson j;
    j["type"] = "map_save";
    j["items"];

    SaveMap(gctx, gctx->mMapm->mSubMap, j["items"]["submap"]);
    SaveMap(gctx, gctx->mMapm->mOverMap, j["items"]["overmap"]);
    SaveMap(gctx, gctx->mMapm->mCityMap, j["items"]["citymap"]);

    out << j.dump(4);
    out.close();
}

void MapIO::SaveMapEntities(Map *map, ojson &mapdata)
{
    mapdata["npcs"] = ojson::array();
    for (Entity* ent : map->mNpcs) {
        mapdata["npcs"] += ent->mId;
    }

    mapdata["pawns"] = ojson::array();
    for (Entity* p : map->mPawns) {
        mapdata["pawns"] += p->mId;
    }
}
void MapIO::SaveMapItems(Map *map, ojson &mapdata)
{
    mapdata["items"] = ojson::array();
    for (auto pair : map->mItemStackMap) {

        ItemIO::SaveItemStack(pair.second, mapdata["items"]);
    }
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
        j["items"][p->mName] = ojson::object();
        AddCommonEntityStat(p, j["items"][p->mName]);
        j["items"][p->mName] += {"type", p->mType};
        j["items"][p->mName] += {"skill", GetSkillData(p)};
        j["items"][p->mName] += {"quick_skill", GetQSkillData(p)};
        j["items"][p->mName] += {"equipments", GetEqquipedEqData(p)};
        j["items"][p->mName] += {"inventory", GetInvData(p)};
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

        gc->mObjm->mEntm->mFocusedPc = p;
        gc->mObjm->mEntm->mFocusedEnt = p;

        LoadCommonEntityStat(p, pdata);

        EntityManager* entm = gc->mObjm->mEntm;
        for (ojson eqdata : pdata["equipments"]) {
            Equipment* eq {nullptr};
            entm->EquipItem(*gc, p, ItemIO::LoadEquipment(gc, eqdata, eq));
        }

        for (ojson itemdata : pdata["inventory"]) {
            Item* item = ItemIO::LoadItem(gc, itemdata, nullptr);
            if (!item) continue;
            entm->PickUpItem(*gc, p, item);
        }
    }

}

void CharacterIO::SaveNpc(GameContext *gc, string name)
{
    using json = nlohmann::json;
    ofstream out;

    out.open("save/userdata/" + name + "/npc.json");

    ojson j;
    j["type"] = "npc_save";
    j["items"];

    for (Entity* ent : gc->mObjm->mEntm->mEntTable) {
        if (ent->mCode == "") continue;
        j["items"][ent->mId] = ojson::object();
        ojson& entsave = j["items"][ent->mId];
        AddCommonEntityStat(ent, entsave);
        entsave += {"demeanor", ent->mDemeanor};
        entsave += {"skill", GetSkillData(ent)};
        entsave += {"equipments", GetEqquipedEqData(ent)};
    }

    out << j.dump(4);
    out.close();
}

void CharacterIO::LoadNpc(GameContext *gc, string name)
{
    json save;
    JsonHelper::LoadJsonFile(save, "save/userdata/" + name + "/npc.json");

    for (json entdata : save["items"]) {
        gc->mObjm->mEntm->AllocEntityOnTable(
            gc, entdata["code"].get<string>(), entdata["map_x"].get<int>(),
            entdata["map_y"].get<int>(), entdata["id"].get<int>()
        );

        Entity* ent = gc->mObjm->mEntm->mEntTable[entdata["id"].get<int>()];

        ent->mDemeanor = entdata["demeanor"].get<Demeanor>();
        LoadCommonEntityStat(ent, entdata);
        LoadSkillData(ent, entdata);

        EntityManager* entm = gc->mObjm->mEntm;
        for (ojson eqdata : entdata["equipments"]) {
            Equipment* eq {nullptr};
            entm->EquipItem(gc, ent, ItemIO::LoadEquipment(gc, eqdata, eq));
        }
    }

}

void CharacterIO::AddCommonEntityStat(Entity *ent, ojson& oj)
{
    oj += {"name", ent->mName};
    oj += {"code", ent->mCode};
    oj += {"id", ent->mId};
    oj += {"is_on_map", ent->mIsOnMap};
    oj += {"tile_id", ent->mTileId};
    oj += {"map_x", ent->mMapX};
    oj += {"map_y", ent->mMapY};

    oj += {"str", ent->mStr};
    oj += {"end", ent->mEnd};
    oj += {"dex", ent->mDex};
    oj += {"per", ent->mPer};
    oj += {"agi", ent->mAgi};
    oj += {"wil", ent->mWil};
    oj += {"int", ent->mInt};
    oj += {"spd", ent->mSpd};

    oj += {"cur_hp", ent->mCurHp};
    oj += {"cur_sp", ent->mCurSp};
    oj += {"cur_ap", ent->mCurAp};

    oj += {"weight", ent->mWeight};
}

void CharacterIO::LoadCommonEntityStat(Entity *ent, const ojson &entdata)
{
    ent->mIsOnMap = entdata["is_on_map"].get<bool>();
    ent->mTileId = entdata["tile_id"].get<int>();
    ent->mMapX = entdata["map_x"].get<int>();
    ent->mMapY = entdata["map_y"].get<int>();

    ent->mStr = entdata["str"].get<int>();
    ent->mEnd = entdata["end"].get<int>();
    ent->mDex = entdata["dex"].get<int>();
    ent->mPer = entdata["per"].get<int>();
    ent->mAgi = entdata["agi"].get<int>();
    ent->mWil = entdata["wil"].get<int>();
    ent->mInt = entdata["int"].get<int>();
    ent->mSpd = entdata["spd"].get<int>();

    ent->mCurHp = entdata["cur_hp"].get<int>();
    ent->mCurSp = entdata["cur_sp"].get<int>();
    ent->mCurAp = entdata["cur_ap"].get<int>();

    ent->mWeight = entdata["weight"].get<float>();
}

ojson CharacterIO::GetSkillData(Entity* ent)
{
    ojson ret;

    for(Skill* sk : ent->mSkills) {
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

void CharacterIO::LoadSkillData(Entity *ent, const ojson &entdata)
{
    for (ojson sd : entdata["skill"]) {
        std::string skillcode = sd.get<std::string>();
        Skill* skill = new Skill(skillcode, skillcode);
        ent->mSkills.push_back(skill);
    }
}

ojson ItemIO::GetItemData(Item *item)
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

void ItemIO::AddCommonItemData(Item *item, ojson &oj)
{
    oj += {"code", item->mCode};
    oj += {"name", item->mName};
    oj += {"type", item->mType};
    oj += {"id", item->mId};
    oj += {"tile_id", item->mTileId};
    oj += {"value", item->mValue};
    oj += {"weight", item->mWeight};
}

ojson ItemIO::GetEqData(Equipment *eq)
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

ojson ItemIO::GetConsumableData(Consumable *cons)
{
    ojson ret = ojson::object();

    AddCommonItemData(cons, ret);

    return ret;
}

ojson CharacterIO::GetEqquipedEqData(Entity* ent)
{
    ojson ret;

    for (auto eqPair : ent->mEqs) {
        if (eqPair.second == nullptr) continue;
        Equipment* eq = eqPair.second;
        ret += ItemIO::GetEqData(eq);
    }

    return ret;
}

ojson CharacterIO::GetInvData(Pawn *p)
{
    ojson ret;

    for (auto itemPair : p->mInventory) {
        ret += ItemIO::GetItemData(itemPair.second);
    }

    return ret;
}



void GameIO::MakeUserSave(string name)
{
    std::filesystem::create_directories("save/userdata/" + name);
}

void ItemIO::SaveItemStack(ItemStack *stack, ojson &stackdata)
{
    for (Item* item : stack->mStack) {
        stackdata += GetItemData(item);
    }
}

Item* ItemIO::LoadItem(GameContext* gctx, const json &itemdata, Item *item)
{
    if (itemdata["type"].get<ItemType>() == ItemType::Equipment) {
        return ItemIO::LoadEquipment(gctx, itemdata, (Equipment*) item);
    }
    else if (itemdata["type"].get<ItemType>() == ItemType::Consumable) {
        return ItemIO::LoadConsumable(gctx, itemdata, (Consumable*) item);
    }
    else {
        SDL_Log("[WARNING] load item: unknown item type!");
        return nullptr;
    }
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

Equipment *ItemIO::LoadEquipment(GameContext* gctx, json eqdata, Equipment* eq)
{
    using namespace std;
    eq = new Equipment(*gctx->mObjm,
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

Consumable *ItemIO::LoadConsumable(GameContext *gctx, json consdata, Consumable* cons)
{
    using namespace std;
    cons = new Consumable(*gctx->mObjm, consdata["code"].get<string>());
    cons->mTileId = consdata["tile_id"].get<int>();

    return cons;
}
