#pragma once

class GameContext;
class Entity;
class ObjectManager;
class UIManager;

using json = nlohmann::json;

class Skill {
    public:
    Skill(std::string code, std::string name);
    std::string mCode {""}; //스킬이 가지는 코드
    std::string mName {""}; //스킬의 실제 이름

    //스킬을 사용하는 주체나 방법에 따라 메서드를 오버로딩 시킬 수 있다.
    void Activate(SkillManager* skm);
};


class SkillManager {
    public:
    SkillManager(GameContext& gc);
    //데이터 로드
    void LoadJsonData();
    //스킬 활성화
    void ActivateSkill();

    //세터 함수
    void SetSkill(Skill* skill); //실제 스킬 객체 설정
    void SetSkillData(json skilldata); //데이터베이스 상의 스킬 데이터 설정
    void SetTargets(std::vector<Entity*> targets);
    void SetTileIds(std::vector<int> tileIds);
    void SetActor(Entity* actor);
    void SetMap(Map* map);
    //이벤트 핸들링
    void HandleEvent(SDL_Event& e, GameContext& gc);

    //스킬 사용을 위해 필요한 데이터들
    json mSkillDb;
    json mSkillData;
    std::vector<Entity*> mTargets;
    std::vector<int> mTileIds;
    Skill* mSkill {nullptr};
    Entity* mActor {nullptr};
    Map* mMap {nullptr};

    //참조용 컨텍스트
    GameContext* mGc {nullptr};

    //제어용 변수
    bool mIsSkillReady {false};
};

class SkillHelper {
    public:
    //타겟팅 관련
    json GetSkillData(const json& skillDb, std::string code);
    std::string GetSkillTargetType(json skillData, Skill* skill);
    std::string GetSkillName(json skillData, Skill* skill);
    int GetSkillTargetNum(json skillData, Skill* skill); //멀티타겟의 경우는 아직 안만듬

    //스킬 범위
    int GetSkillRange(json skillData, Skill* skill);
    //스킬 데미지 
    int GetSkillDamage(json skillData, Skill* skill, Entity* ent);
    //스킬 코스트
    int GetHpUse(json skillData, Skill* skill, Entity* ent);
    int GetSpUse(json skillData, Skill* skill, Entity* ent);
    int GetApUse(json skillData, Skill* skill, Entity* ent);
};