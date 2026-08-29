#pragma once

#include "item_enum.h"
#include "item.h"

using json = nlohmann::json;

struct ItemStack;
struct GameContext;
class Map;
class Texture;
class Entity;

class ItemManager {
    public:
    ItemManager(GameContext* gc);

    Item* MakeItem(ObjectManager& objm, std::string code, ItemType);
    Equipment* MakeEq(ObjectManager& objm, std::string code, EqType);

    void LoadSpriteSheets();
    void LoadItemData();

    int GetValidId();
    void ReturnId(int id);

    //스택 관리
    void StackItemOnMap(Map* map, int tileId, Item* item);
    Item* PopSpecificItem(Map* map, int tileId, int itemId);
    Item* PopItemStack(Map* map, int tileId);
    
    //아이템 스폰
    void SpawnItemOnMap(Map* map, int tileId, Item* item);
    void DespawnItemOnMap(Map* map, int tileId, int itemId);

    //이벤트 핸들링
    void HandleEvent(SDL_Event& e, GameContext* gc, float mx, float my);
    void HandleItemEvent(SDL_Event& e, GameContext* gc, Item* item, float mx, float my);
    bool mCanHandleEvent {true};

    //아이템 렌더링
    void RenderItem(Item* item);
    void RenderItem(Item* item, float x, float y, float w, float h);
    void RenderItems();
    void StoreTexture();
    void RenderStoredTex();
    void Render();
    
    bool mIsRender {true};
    bool mIsRenderUpdate {false};
    
    //참고용 컨텍스트
    GameContext* mGc {nullptr};

    //임시 텍스처
    SDL_Texture* mTempTex {nullptr};
    //아이템 스프라이트 시트
    Texture* mConsumableSs {nullptr};
    Texture* mWeaponSs {nullptr};
    Texture* mGearSs {nullptr};

    //아이템 아이디 관리
    inline const static int kMaxItemId {2048};
    int mIdTable[kMaxItemId];

    //아이템 json 데이터객체들
    json mItemCodeTable;

    json mWeaponDb;
    json mUWeaponDb;
    json mWeaponSsMap;

    json mGearDb;
    json mUGearDb;
    json mGearSsMap;

    json mUseItDb;
    json mConsumItDb;
    json mConsumItSsMap;

    json mSpecItDb;
};

class ItemHelper {
    public:
    void DestroyStackObj(Map* map, int tileId);

    EqType GetEqType(json eq);
    ItemType GetItemType(std::string iType);
    float GetEqWeight(json eq); 
    float GetConsWeight(json cons);

    int GetEqDamage(json eq);
    int GetEqDamage(json eq, Entity* ent);
    int GetEqArmor(json eq);
    
    json* GetEqDb(ItemManager* itm, std::string code);
    json* GetEqDb(ItemManager* itm, Equipment* eq);
    json* GetItemDb(ItemManager* itm, json scenarioData);
    json* GetItemDb(ItemManager* itm, Item* item);
    
    json* GetItemSsMap(ItemManager* itm, Item* item);
    Texture* GetItemSs(ItemManager* itm, Item* item);
    
};