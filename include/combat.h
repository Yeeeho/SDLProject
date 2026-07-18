#pragma once

#include <vector>

class UIManager; class ObjectManager;
class Entity;

class CombatManager {
    public:

    void EnterCombatState();
    void ExitCombatState();

    void Update(UIManager& uim, ObjectManager& objm);
    bool mIsUpdate {true};

    bool mIsCombatState {false}; //전투 상태 플래그
};