#pragma once

#include <string>

enum class ItemType {
    Consumable, Artifact, Equipment
};

//전방 선언
class ObjectManager;
//아이템 최상위 인터페이스 클래스
class Item {
    public:
    Item() = default;
    Item(std::string name);

    //아이템 속성
    std::string mName = ""; //이름
    int mValue = 0; //가치
};

enum class EqType {
    Head, Torso, Leg, Hand, Foot, Weapon, Offhand
};

//장비 클래스
class Equipment : public Item {
    public:
    Equipment(const ObjectManager& objm, std::string name);

    //장비 속성
    int mDamage = 0;
    int mArmor = 0;
};