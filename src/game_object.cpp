#include "pch.h"

#include "game_object.h"
#include "game_json.h"
#include "item/item.h"
#include "item/item_manager.h"
#include "skill/skill.h"
#include "skill/skill_enum.h"
#include "ui.h"
#include "text.h"
#include "city.h"
#include "entity.h"
#include "map.h"
#include "system.h"

ObjectManager::ObjectManager()
{
    mJsm = new JsonManager();
    mItm = new ItemManager();
}

void ObjectManager::RenderObjects()
{
    SDL_SetRenderLogicalPresentation(System::sRenderer, 1280, 720, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

void ObjectManager::DestroyObjects()
{
    //오브젝트 파괴하는 함수. 아직 미구현.
}
