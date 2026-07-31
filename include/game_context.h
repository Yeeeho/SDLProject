#pragma once

class GameStateManager;
class ScenarioManager;
struct TurnManagers;
class RenderManager;
class ObjectManager;
class UIManager;
class SkillManager;

struct GameContext {
    GameContext();
    RenderManager* mRenderM {nullptr};
    GameStateManager* mGsm {nullptr};
    ScenarioManager* mScm {nullptr};
    TurnManagers* mTms {nullptr};
    ObjectManager* mObjm {nullptr};
    UIManager* mUim {nullptr};
    SkillManager* mSkm {nullptr};
};