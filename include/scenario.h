#pragma once

#include <json.hpp>

using json = nlohmann::json;

class UIManager;
class ObjectManager;

//디폴트 시나리오.
class Scenario {
    public:
    Scenario() = default;
    virtual ~Scenario() = default;

    //맵에 정보 로드
    virtual void LoadSubMap(ObjectManager& objm);
    virtual void LoadOverMap(ObjectManager& objm);
    virtual void LoadCityMap(ObjectManager& objm);
    //대화 로드
    virtual void LoadDialogue(ObjectManager& objm);

    //업데이트 로직
    virtual void Update(UIManager& uim, ObjectManager& objm);
    //공용 대화 업데이트 메서드
    void UpdateDialogue(UIManager& uim, ObjectManager& objm);

    bool mIsDialogueUpdate {true};
    int mDialogueProgress {0};
    bool mDialogueEnd {false};
};

class DefScenario : public Scenario {
    public:

    void LoadSubMap(ObjectManager& objm) override;
    void LoadOverMap(ObjectManager& objm) override;
    void LoadCityMap(ObjectManager& objm) override;
    void LoadDialogue(ObjectManager& objm) override;

    void Update(UIManager& uim, ObjectManager& objm) override;
};

class NGScenario : public Scenario {
    public:

    void LoadSubMap(ObjectManager& objm) override;
    void LoadOverMap(ObjectManager& objm) override;
    void LoadCityMap(ObjectManager& objm) override;
    void LoadDialogue(ObjectManager& objm) override;

    void Update(UIManager& uim, ObjectManager& objm) override;
};

class ScenarioManager {
    public:
    ScenarioManager();

    //현재 시나리오 설정
    void SetCurrentScenario(Scenario* sc, ObjectManager& objm);
    void DestroyCurrentScenario();

    //이벤트 핸들랑
    void HandleEvent(SDL_Event& e, UIManager& uim, ObjectManager& objm, float mouseX, float mouseY);
    //업데이트
    void Update(UIManager& uim, ObjectManager& objm);

    void LoadThings(ObjectManager& objm);
    void ClearThings(ObjectManager& objm);

    Scenario* mCurrentSc {nullptr};
    private:
    Scenario* mNextSc {nullptr};
};