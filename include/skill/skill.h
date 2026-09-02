#pragma once

class Item;
class GameContext;
class Entity; class Npc;
class ObjectManager;
class UIManager;

using json = nlohmann::json;

class Skill {
    public:
    Skill(std::string code, std::string name);
    std::string mCode {""}; //스킬이 가지는 코드
    std::string mName {""}; //스킬의 실제 이름

    void Activate(SkillManager* skm);
};

//스킬 큐에 넣기 위한 참고용 컨텍스트
struct SkillContext {
    public:
    SkillContext(Skill*, std::vector<Item*>& targetItems, std::vector<Entity*>& targetEnts, std::vector<int>& tileIds);
    SkillContext(Skill*, const std::vector<int>& tileIds, Map* map);

    void Activate(GameContext* gctx, Entity* actor);

    Skill* mSkill {nullptr}; //해제하지마라
    std::vector<Item*> mTargetItems;
    std::vector<Entity*> mTargetEnts;
    std::vector<int> mTargetTIds;
    Map* mMap {nullptr}; //해제하지마라
};

namespace SkillAction {
    bool MoveAction(GameContext* gctx, Entity* actor, SkillContext* skCtx);
    bool MoveAction(GameContext* gctx, Entity* actor, Skill* skill, std::vector<int>& tileIds, Map* map);
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
    void SetTargetItems(std::vector<Item*> targetItems);
    void SetTargetItem(Item* item);
    void SetTargets(std::vector<Entity*> targets);
    void SetTarget(Entity* target);
    void SetTileIds(std::vector<int>& tileIds);
    void SetActor(Entity* actor);
    void SetMap(Map* map);
    //이벤트 핸들링
    void HandleEvent(SDL_Event& e, GameContext& gc);

    //스킬 컨텍스트에 로드하기 위해 임시로 캐싱하는 목적
    json mSkillDb;
    json mSkillData;
    Skill* mSkill {nullptr};
    std::vector<Item*> mTargetItems;
    std::vector<Entity*> mTargets;
    std::vector<int> mTileIds;
    Entity* mActor {nullptr};
    Map* mMap {nullptr};

    //참조용 컨텍스트
    GameContext* mGc {nullptr};

    //제어용 변수
    bool mIsSkillReady {false};
};

namespace SkillHelper {
    json GetSkillData(const json& skillDb, std::string code); //스킬 데이터 자체 반환

    std::string GetSkillType(GameContext* gctx, std::string code); //스킬 타입
    std::string GetSkillName(json skillData, Skill* skill); //스킬 이름
    std::string GetSkillTargetType(json skillData, Skill* skill);

    int GetSkillTargetNum(json skillData, Skill* skill); //멀티타겟의 경우는 아직 안만듬

    int GetSkillRange(json skillData, Skill* skill); //스킬 범위
    int GetSkillDamage(json skillData, Skill* skill, Entity* ent); //스킬 데미지
    //스킬 코스트
    int GetHpUse(json skillData, Skill* skill, Entity* ent);
    int GetSpUse(json skillData, Skill* skill, Entity* ent);
    int GetApUse(json skillData, Skill* skill, Entity* ent);

    //특정 타입의 스킬을 반환해주는 메서드
    Skill* GetMovementSkill(Entity* ent); //이동형 스킬이 있다면 스킬을 반환하고, 없다면 널포인터를 반환함.
};