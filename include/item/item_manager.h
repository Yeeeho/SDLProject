#pragma once

#include "item_enum.h"
#include "item.h"

using json = nlohmann::json;

struct GameContext;
class Map;
class Texture;
class Entity;

class ItemManager {
    public:
    ItemManager(GameContext* gc);

    Item* MakeItem(ObjectManager& objm, std::string code, ItemType);
    
    void LoadSpriteSheets();
    void LoadItemData();

    void SpawnItemOnMap(Item* item, std::map<int, Item*>& mapItems, int tileId);
    void SpawnItemOnMap(Item* item, std::map<int, Item*>& mapItems, Map* map, int xPos, int yPos);
    void DespawnItemOnMap(int itemId, std::map<int, Item*>& mapItems);

    int GetValidId();
    void ReturnId(int id);

    void RenderItem(Item* item);
    void RenderItems();
    void StoreTexture();
    void RenderStoredTex();
    void Render();

    bool mIsRender {true};
    bool mIsRenderUpdate {false};

    //참고용 컨텍스트
    GameContext* mGc {nullptr};

    //맵 상에 존재하는 아이템들의 컨테이너
    std::map<int, Item*> mSubmapItems;
    std::map<int, Item*> mCitymapItems;

    //임시 텍스처
    SDL_Texture* mTempTex {nullptr};
    //아이템 스프라이트 시트
    Texture* mConsumableSs {nullptr};

    int mIdTable[1024];

    //아이템 json 데이터객체들
    json mItemCodeTable;

    json mWeaponDb;
    json mUWeaponDb;
    json mWeaponSsMap;

    json mGearDb;
    json mUGearDb;

    json mUseItDb;
    json mConsumItDb;
    json mConsumItSsMap;

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
    json* GetItemDb(ItemManager* itm, json scenarioData);
    json* GetItemSsMap(ItemManager* itm, Item* item);
    Texture* GetItemSs(ItemManager* itm, Item* item);
    json* GetItemDb(ItemManager* itm, Item* item);
    json* GetEqDb(ItemManager* itm, Equipment* eq);
    ItemType GetItemType(std::string iType);
};