#include "pch.h"

#include "entity.h"
#include "item.h"

Team::Team()
{
}

Entity::Entity(std::string name)
{
    SDL_Log("this is abstract entity");
}

Pawn::Pawn(std::string name)
{  
    mName = name;

    mMaxHp = 100;
    mMaxAp = 100;
    mMaxSpd = 100;
    mMaxAtk = 10;
    mMaxArmor = 10;
}

Enemy::Enemy(std::string name)
{
}
