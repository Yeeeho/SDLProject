#pragma once

#include <json.hpp>

using json = nlohmann::json;

struct GameContext;
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
    virtual void LoadScenarioData(GameContext& gc);
    virtual void LoadSubMap(GameContext& gc);
    virtual void LoadOverMap(GameContext& gc);
    virtual void LoadCityMap(GameContext& gc);

    virtual void Update(GameContext& gc);

    //비 가상함수들
    //시나리오 진행 업데이트
    void UpdateScenario(GameContext& gc);
    bool mIsScUpdate {true};
    int mScProgress {0};

    //공용 대화 업데이트 메서드
    void UpdateDialogue(GameContext& gc, json d);
    int mDialogueProgress {0};
    bool mIsDialogueUpdate {true};
    bool mIsDialogueEnd {true}; //하나의 대화 객체가 끝났을때, 기본값 건들지 말것
};

class DefScenario : public Scenario {
    public:

    void LoadScenarioData(GameContext& gc);
    void LoadSubMap(GameContext& gc) override;
    void LoadOverMap(GameContext& gc) override;
    void LoadCityMap(GameContext& gc) override;

    void Update(GameContext& gc) override;

};

class NGScenario : public Scenario {
    public:
    
    void LoadScenarioData(GameContext& gc) override;
    void LoadSubMap(GameContext& gc) override;
    void LoadOverMap(GameContext& gc) override;
    void LoadCityMap(GameContext& gc) override;

    void Update(GameContext& gc) override;
    
    private:
};

class ScenarioManager {
    public:
    ScenarioManager();

    //현재 시나리오 설정
    void SetCurrentScenario(Scenario* sc, GameContext& gc);
    void DestroyCurrentScenario();

    //이벤트 핸들랑
    void HandleEvent(SDL_Event& e, GameContext& gc, float mouseX, float mouseY);
    //업데이트
    void Update(GameContext& gc);

    void LoadThings(GameContext& gc); //상태가 바뀔 때 한번 로드됨
    void ClearThings(GameContext& gc);

    Scenario* mCurrentSc {nullptr};
    private:
    Scenario* mNextSc {nullptr};
};