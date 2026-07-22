#pragma once

class Entity;

class Skill {
    public:

    //스킬을 사용하는 주체나 방법에 따라 메서드를 오버로딩 시킬 수 있다.
    virtual void Activate(Entity* ent);
};

class SelfTargetSkill : public Skill {
    public:
    void Activate(Entity* ent) override;
};

class SingleTargetSkill : public Skill {
    public:
    void Activate(Entity* ent) override;
};

class MultiTargetSkill : public Skill {
    public:
    void Activate(Entity* ent) override;
};

class AreaSkill : public Skill {
    public:
    void Activate(Entity* ent) override;
};

class CombinedSkill : public Skill {
    public:
    void Activate(Entity* ent) override;
};

enum class SkillType {
    Single, Multi, Area, Combined
};

using json = nlohmann::json;

class SkillManager {
    public:
    SkillManager();

    void LoadJsonData();
    json mSingleTargetSkillData;
    json mMultiTargetSkillData;
    json mAreaSkillData;
    json mCombinedSkillData;

    Skill* mCurrentSkillState {nullptr};

    void SetCurrentSkillState(SkillType);
    void ActivateSkill(Entity ent);
};
