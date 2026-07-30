#include "pch.h"

#include "item/item.h"
#include "item/item_manager.h"
#include "game_json.h"
#include "game_object.h"

Item::Item(std::string name)
{
    SDL_Log("instantiated item interface class, which is not likely");
}

Equipment::Equipment(const ObjectManager& objm, std::string code)
{
    ItemHelper ith;
    json* db = ith.GetEqDb(objm.mItm , code);

    json equipmentItems = (*db)["items"];
    json eq;
    //이름이 json파일에 있는지 검사
    if (equipmentItems.contains(code)) {
        //이름이 포함되었음
        eq = equipmentItems[code];
    }
    else {
        //이름이 파일에 없으면 error데이터 할당
        code = "error_equipment";
        eq = equipmentItems[code];
    }

    //데이터 가져오기
    mName = eq["name"].get<std::string>();
    mCode = code;

    mEqType = ith.GetEqType(eq);

    mWeight = ith.GetEqWeight(eq);

    mDamage = ith.GetEqDamage(eq);
    mArmor = ith.GetEqArmor(eq);
}

