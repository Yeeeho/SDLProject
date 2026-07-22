#include "pch.h"

#include "item.h"
#include "game_json.h"
#include "game_object.h"

Item::Item(std::string name)
{
}

Equipment:: Equipment(const ObjectManager& objm, std::string name)
{
    const json& equipmentItems = objm.mJsm->mEquipmentDb["items"];

    json eq;

    //이름이 json파일에 있는지 검사
    if (equipmentItems.contains(name)) {
        //이름이 포함되었음
        eq = equipmentItems[name];
    }
    else {
        //이름이 파일에 없으면 error데이터 할당
        name = "error_equipment";
        eq = equipmentItems[name];
    }

    mName = eq["name"].get<std::string>();
}
