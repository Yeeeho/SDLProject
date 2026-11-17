#pragma once

class SpriteSheetManager;
class MapManager;
class GameStateManager;
class ScenarioManager;
class TurnManager;
class RenderManager;
class ObjectManager;
class UIManager;
class SkillManager;
struct EventContext;

struct GameContext {
    GameContext();
    SpriteSheetManager* mSpsm {nullptr};
    RenderManager* mRenderM {nullptr};
    GameStateManager* mGsm {nullptr};
    ScenarioManager* mScm {nullptr};
    TurnManager* mTurnm {nullptr};
    MapManager* mMapm {nullptr};
    ObjectManager* mObjm {nullptr};
    UIManager* mUim {nullptr};
    SkillManager* mSkm {nullptr};

    EventContext* mEvCtx {nullptr};
};