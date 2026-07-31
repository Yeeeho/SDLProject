#include "pch.h"

#include "render.h"
#include "game_context.h"
#include "game_state.h"
#include "game_object.h"
#include "scenario.h"
#include "turn.h"
#include "ui.h"
#include "skill/skill.h"

GameContext::GameContext()
{
    mRenderM = new RenderManager();
    mScm = new ScenarioManager();
    mTms = new TurnManagers();
    mObjm = new ObjectManager();
    mUim = new UIManager(*this);
    mSkm = new SkillManager(*this);
    mGsm = new GameStateManager();
}