#pragma once

#include <string>
#include <unordered_map>


class Team {
    public:
    Team();
    
    int supply = 0;
    
    Entity* teammates{nullptr};
};

class Item;
class Equipment;
enum class EqType;

class Entity {
    public:
    Entity() = default;
    Entity(std::string name);

    int maxHp;
    int curHp;

    int maxAp;
    int curAp;

    int maxSpd;
    int curSpd;

    int maxAtk;
    int curAtk;

    int maxArmor;
    int curArmor;
    
    std::unordered_map<EqType, Equipment*> eqs;
};


class Pawn : public Entity {
    public:
    Pawn(std::string name);
};

class Enemy : public Entity {
    public:
    Enemy(std::string name);
};