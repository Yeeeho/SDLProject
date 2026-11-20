#include "pch.h"

#include "texture.h"
#include "animation.h"
#include "render.h"
#include "game_context.h"
#include "game_state.h"
#include "map.h"
#include "game_object.h"
#include "scenario.h"
#include "turn.h"
#include "ui.h"
#include "event_context.h"
#include "skill/skill.h"

GameContext::GameContext()
{
    mSpsm = new SpriteSheetManager();
    mAnim = new AnimationManager(mSpsm);
    mRenderM = new RenderManager();
    mScm = new ScenarioManager();
    mTurnm = new TurnManager(this);
    mMapm = new MapManager();
    mObjm = new ObjectManager(this);
    mUim = new UIManager(*this);
    mSkm = new SkillManager(*this);
    mGsm = new GameStateManager();

    mEvCtx = new EventContext();
}