#pragma once

class ObjectManager;

//디폴트 시나리오.
class Scenario {
    public:
    Scenario() = default;
    virtual ~Scenario() = default;

    //엔티티 로드
    virtual void LoadEntities(ObjectManager& objm) = 0;
    //맵에 정보 로드
    virtual void LoadSubMap(ObjectManager& objm) = 0;
    virtual void LoadOverMap(ObjectManager& objm) = 0;
    virtual void LoadCityMap(ObjectManager& objm) = 0;
};

class DefScenario : public Scenario {
    public:
    void LoadEntities(ObjectManager& objm) override;

    void LoadSubMap(ObjectManager& objm) override;
    void LoadOverMap(ObjectManager& objm) override;
    void LoadCityMap(ObjectManager& objm) override;
};

class NGScenario : public Scenario {
    public:
    
    void LoadEntities(ObjectManager& objm) override;

    void LoadSubMap(ObjectManager& objm) override;
    void LoadOverMap(ObjectManager& objm) override;
    void LoadCityMap(ObjectManager& objm) override;
};

class ScenarioManager {
    public:
    ScenarioManager();

    void SetCurrentScenario(Scenario* sc);
    void DestroyCurrentScenario();

    void LoadThings(ObjectManager& objm);
    void ClearThings(ObjectManager& objm);

    private:
    Scenario* mCurrentSc {nullptr};
    Scenario* mNextSc {nullptr};
};