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
    //json 데이터
    json mData;
    json mDialogue;
    json mDialogueSection;

    //맵에 정보 로드
    virtual void LoadScenarioData(ObjectManager& objm);
    virtual void LoadSubMap(ObjectManager& objm);
    virtual void LoadOverMap(ObjectManager& objm);
    virtual void LoadCityMap(ObjectManager& objm);

    virtual void Update(UIManager& uim, ObjectManager& objm, GameStateManager& gsm);

    //비 가상함수들
    //시나리오 진행 업데이트
    void UpdateScenario(UIManager& uim, ObjectManager& objm, GameStateManager& gsm);
    bool mIsScUpdate {true};
    int mScProgress {0};

    //공용 대화 업데이트 메서드
    void UpdateDialogue(UIManager& uim, ObjectManager& objm, json d);
    int mDialogueProgress {0};
    bool mIsDialogueUpdate {true};
    bool mIsDialogueEnd {true}; //하나의 대화 객체가 끝났을때, 기본값 건들지 말것
};

class DefScenario : public Scenario {
    public:

    void LoadScenarioData(ObjectManager& objm);
    void LoadSubMap(ObjectManager& objm) override;
    void LoadOverMap(ObjectManager& objm) override;
    void LoadCityMap(ObjectManager& objm) override;

    void Update(UIManager& uim, ObjectManager& objm, GameStateManager& gsm) override;

};

class NGScenario : public Scenario {
    public:
    
    void LoadScenarioData(ObjectManager& objm) override;
    void LoadSubMap(ObjectManager& objm) override;
    void LoadOverMap(ObjectManager& objm) override;
    void LoadCityMap(ObjectManager& objm) override;

    void Update(UIManager& uim, ObjectManager& objm, GameStateManager& gsm) override;
    
    private:
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
    void Update(UIManager& uim, ObjectManager& objm, GameStateManager& gsm);

    void LoadThings(ObjectManager& objm); //상태가 바뀔 때 한번 로드됨
    void ClearThings(ObjectManager& objm);

    Scenario* mCurrentSc {nullptr};
    private:
    Scenario* mNextSc {nullptr};
};