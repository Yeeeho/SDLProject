#pragma once

#include <string>

enum class ItemType {
    Consume, General
};

class Item {
    public:
    Item() = default;
    Item(std::string name);
    int value;
};

enum class EqType {
    Head, Torso, Leg, Hand, Foot, Weapon, Offhand
};

class Equipment : public Item {
    public:
    Equipment(std::string name);
};