#pragma once

class GameContext;
class Entity;
class ObjectManager;
class UIManager;

using json = nlohmann::json;

class Skill {
    public:
    Skill(std::string code);
    std::string mCode; //스킬이 가지는 코드

    //스킬을 사용하는 주체나 방법에 따라 메서드를 오버로딩 시킬 수 있다.
    virtual void Activate(
        GameContext& gc,
        json skillData, Entity* actor, std::vector<Entity*> targets,
        Map* map, int targetTileId
    );
};


class SkillManager {
    public:
    SkillManager(GameContext& gc);
    //데이터 로드
    void LoadJsonData();
    
    //세터 함수
    void SetSkill(Skill* skill); //실제 스킬 객체 설정
    void SetSkillData(json skilldata); //데이터베이스 상의 스킬 데이터 설정
    void SetActor(Entity* actor);
    void SetTargets(std::vector<Entity*> targets);
    void SetMap(Map* map);
    void SetTargetTileId(int tid);
    //이벤트 핸들링
    void HandleEvent(SDL_Event& e, GameContext& gc);


    //스킬 사용을 위해 필요한 데이터들
    json mSkillDb;
    json mSkillData;
    Skill* mSkill {nullptr};
    Entity* mActor {nullptr};
    std::vector<Entity*> mTargets;
    Map* mMap {nullptr};
    int mTid {0}; 

    //참조용 컨텍스트
    GameContext* mGc {nullptr};

    //제어용 변수
    bool mIsSkillReady {false};
    void ActivateSkill();
};

class SkillHelper {
    public:
    int GetSkillRange(json skillData, Skill* skill);
    std::string GetSkillTargetType(json skillData, Skill* skill);
    int GetSkillTargetNum(json skillData, Skill* skill); //멀티타겟의 경우는 아직 안만듬
    int GetSkillDamage(json skillData, Skill* skill, Entity* ent);
};