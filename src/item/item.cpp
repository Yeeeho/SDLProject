#include "pch.h"

#include "item/item.h"
#include "item/item_manager.h"
#include "game_json.h"
#include "game_object.h"

int Item::GetValue()
{
    SDL_Log("get item value: virtual function called");
    return 0;
}


Equipment::Equipment(const ObjectManager& objm, std::string code, EqType et)
{
    mId = objm.mItm->GetValidId();
    mType = ItemType::Equipment;
    mEqType = et;

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

    mWeight = ith.GetEqWeight(eq);

    mDamage = ith.GetEqDamage(eq);
    mArmor = ith.GetEqArmor(eq);

    mValue = GetValue();
}

void Equipment::Destroy(const ObjectManager &objm)
{
    objm.mItm->ReturnId(mId);
}

int Equipment::GetValue()
{
    SDL_Log("get equipment value: still making");
    return 0;
}

Consumable::Consumable(const ObjectManager &objm, std::string code)
{
    ItemHelper ih;
    mId = objm.mItm->GetValidId(); //아이디 테이블을 조회해서 유효한 아이디를 가져온다.
    mType = ItemType::Consumable;
    
    json consumTb = objm.mItm->mConsumItDb["items"];
    
    if (!consumTb.contains(code)) {
        SDL_Log("consumable: item code not found in db");
        code = "error_item";
    }
    
    mCode = code;
    
    json item = consumTb[code];
    mName = item["name"].get<std::string>();
    mValue = GetValue();
}

void Consumable::Destroy(const ObjectManager &objm)
{
    objm.mItm->ReturnId(mId);
}

int Consumable::GetValue() 
{
    SDL_Log("get consumable value: still making");
    return 0;
}
