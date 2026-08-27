#include "pch.h"

#include "game_context.h"
#include "game_object.h"
#include "system/game_json.h"
#include "item/item.h"
#include "item/item_manager.h"
#include "skill/skill.h"
#include "skill/skill_enum.h"
#include "ui.h"
#include "text.h"
#include "city.h"
#include "entity.h"
#include "map.h"
#include "system/system.h"

ObjectManager::ObjectManager(GameContext* gc)
{
    mJsm = new JsonManager();
    mItm = new ItemManager(gc);
}

void ObjectManager::DestroyObjects()
{
    //오브젝트 파괴하는 함수. 아직 미구현.
}
