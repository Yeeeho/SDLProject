#pragma once

class Entity;

class Skill {
    public:
    Skill(std::string code);
    std::string mCode; //스킬이 가지는 코드

    //스킬을 사용하는 주체나 방법에 따라 메서드를 오버로딩 시킬 수 있다.
    virtual void Activate(Entity* actor, std::vector<Entity*> targets, Map* map);
};

using json = nlohmann::json;

class SkillManager {
    public:
    SkillManager();

    void LoadJsonData();
    json mSkillData;

    void SetActor(Entity* actor);
    void SetTargets(std::vector<Entity*> targets);
    void SetMap(Map* map);

    Entity* mActor {nullptr};
    std::vector<Entity*> mTargets;
    Map* mMap {nullptr};

    void ActivateSkill(Skill* skill);
};
