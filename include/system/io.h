#pragma once

#include <string>

struct GameContext;
class Consumable; class Equipment; class Item;
class Pawn;

using json = nlohmann::json;
using ojson = nlohmann::ordered_json;

namespace IOHelper {
    using namespace std;
    
    void MakeDir(string path);

    string Indent(int num);
    string Wrap(string message);
};

namespace CharacterIO {
    using namespace std;

    //플레이어블 캐릭터 입출력
    void SavePawn(GameContext* gc, string name);
    void LoadPawn(GameContext* gc, string name);
    //논 플레이어블 입출력
    void SaveNpc(GameContext* gc, string name);
    void LoadNpc(GameContext* gc, string name);

    //스킬 관련
    ojson GetSkillData(Pawn* p);
    ojson GetQSkillData(Pawn* p);

    //인벤토리 관련 메서드
    ojson GetEqquipedEqData(Pawn* p);
    ojson GetInvData(Pawn* p);

    //아이템 관련 메서드
    ojson GetItemData(Item* item);
    void AddCommonItemData(Item* item, ojson& oj);
    ojson GetEqData(Equipment* eq);
    ojson GetConsumableData(Consumable* cons);
};

namespace ItemIO {
    void LoadCommonItemData(json itemdata, Item* item);
    Equipment* LoadEquipment(GameContext* gctx, json eqdata);
    Consumable* LoadConsumable(GameContext* gctx, json consdata);
};

namespace GameIO {
    using namespace std;

    void Save(GameContext* gc, string name);
    void Load(GameContext* gc, string name);
    void MakeUserSave(string name);
};