#include "pch.h"

#include "skill.h"
#include "game_json.h"

void Skill::Activate(Entity *ent)
{

}

void SelfTargetSkill::Activate(Entity *ent)
{
}

void SingleTargetSkill::Activate(Entity *ent)
{
}

void MultiTargetSkill::Activate(Entity *ent)
{
}

void AreaSkill::Activate(Entity *ent)
{
}

SkillManager::SkillManager()
{
    mCurrentSkillState = new Skill();
}

void SkillManager::LoadJsonData()
{
    JsonHelper jh;
    jh.LoadJsonFile(mSingleTargetSkillData, "data/skill/single_target_skill.json");
    jh.LoadJsonFile(mMultiTargetSkillData, "data/skill/single_target_skill.json");
    jh.LoadJsonFile(mAreaSkillData, "data/skill/single_target_skill.json");
    jh.LoadJsonFile(mCombinedSkillData, "data/skill/single_target_skill.json");
}

void SkillManager::SetCurrentSkillState(SkillType st)
{
    if (mCurrentSkillState) delete mCurrentSkillState;
    
    if (st == SkillType::Single) mCurrentSkillState = new SingleTargetSkill();
    if (st == SkillType::Multi) mCurrentSkillState = new MultiTargetSkill();
    if (st == SkillType::Area) mCurrentSkillState = new AreaSkill();
    if (st == SkillType::Combined) mCurrentSkillState = new CombinedSkill();
}
