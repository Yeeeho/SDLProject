#include "pch.h"

#include "entity.h"
#include "map.h"
#include "skill/skill.h"
#include "game_json.h"

Skill::Skill(std::string code)
{
    mCode = code;
}

void Skill::Activate(Entity *actor, std::vector<Entity *> targets, Map *map)
{
    std::string message = actor->mName + " 이 스킬을 사용하였다.";
    SDL_Log(message.c_str());
}


SkillManager::SkillManager()
{
    LoadJsonData();
}

void SkillManager::LoadJsonData()
{
    JsonHelper jh;
    jh.LoadJsonFile(mSkillData, "data/skill/skill.json");
}

void SkillManager::SetActor(Entity *actor)
{
    mActor = actor;
}

void SkillManager::SetTargets(std::vector<Entity *> targets)
{
    mTargets.clear();
    mTargets = targets;
}

void SkillManager::SetMap(Map *map)
{
    mMap = map;
}

void SkillManager::ActivateSkill(Skill* skill)
{
    if (!mActor) {
        SDL_Log("skill: actor is null!");
        return;
    }

    skill->Activate(mActor, mTargets, mMap);
}
