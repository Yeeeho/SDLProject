#include "pch.h"

#include "game_object.h"
#include "map.h"
#include "system.h"

ObjectManager::ObjectManager()
{
}

void ObjectManager::RenderObjects()
{
    SDL_SetRenderLogicalPresentation(System::sRenderer, 1280, 720, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

void ObjectManager::DestroyObjects()
{
    //오브젝트 파괴하는 함수. 아직 미구현.
}