#pragma once

#include <string>

struct GameContext;
struct ItemStack;
class Consumable; class Equipment; class Item;
class Pawn; class Entity;
class Map; class Grid;

using json = nlohmann::json;
using ojson = nlohmann::ordered_json;

namespace IOHelper {
    using namespace std;
    
    void MakeDir(string path);

    string Indent(int num);
    string Wrap(string message);
    string GetMapTypeName(Map* map);
};

namespace MapIO {
    using namespace std;

    void SaveMaps(GameContext* gctx, string name);
    void SaveMap(GameContext* gctx, Map* map, ojson& mapdata);

    void LoadMaps(GameContext* gctx, string name);
    void LoadMap(GameContext* gctx, Map* map, json& mapdata);

    void SaveMapEntities(Map* map, ojson& mapdata);
    void SaveMapItems(Map* map, ojson& mapdata);
};

namespace CharacterIO {
    using namespace std;

    //플레이어블 캐릭터 입출력
    void SavePawn(GameContext* gc, string name);
    void LoadPawn(GameContext* gc, string name);
    //논 플레이어블 입출력
    void SaveNpc(GameContext* gc, string name);
    void LoadNpc(GameContext* gc, string name);

    void AddCommonEntityStat(Entity* ent, ojson& oj);
    void LoadCommonEntityStat(Entity* ent, const ojson& entdata);

    //스킬 관련
    ojson GetSkillData(Entity* ent);
    ojson GetQSkillData(Pawn* p);
    
    void LoadSkillData(Entity* ent, const ojson& entdata);

    //인벤토리 관련 메서드
    ojson GetEqquipedEqData(Entity* ent);
    ojson GetInvData(Pawn* p);

};

namespace ItemIO {
    void SaveItemStack(ItemStack* stack, ojson& stackdata);

    Item* LoadItem(GameContext* gctx, const json& itemdata, Item* item); //타입 캐스팅 후 로드

    void LoadCommonItemData(json itemdata, Item* item);
    Equipment* LoadEquipment(GameContext* gctx, json eqdata, Equipment* eq);
    Consumable* LoadConsumable(GameContext* gctx, json consdata, Consumable* cons);

    //아이템 관련 메서드
    ojson GetItemData(Item* item);
    void AddCommonItemData(Item* item, ojson& oj);
    ojson GetEqData(Equipment* eq);
    ojson GetConsumableData(Consumable* cons);
};

namespace GameIO {
    using namespace std;

    void Save(GameContext* gc, string name);
    void Load(GameContext* gc, string name);
    void MakeUserSave(string name);
};