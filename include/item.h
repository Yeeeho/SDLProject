#pragma once

#include <string>

enum class ItemType {
    Consumable, Artifact, Equipment
};

class Item {
    public:
    Item() = default;
    Item(std::string name);

    //아이템 속성
    int value; //가치
};

enum class EqType {
    Head, Torso, Leg, Hand, Foot, Weapon, Offhand
};

class Equipment : public Item {
    public:
    Equipment(std::string name);

    //장비 속성
    int mDamage;
    int mArmor;
};