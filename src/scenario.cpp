#include "pch.h"

#include "scenario.h"
#include "game_object.h"
#include "entity.h"


void NGScenario::LoadEntities(ObjectManager& objm)
{
    SDL_Log("load entities for new game scenario at submap");
    objm.mEntm->AllocEntityOnTable(objm, "rat", 1, 1, 0);
    objm.mEntm->AllocPawnOnTable(objm, "nameless_girl", PawnType::Unique, 0);
    SDL_Log("entities loaded");
}

void NGScenario::LoadSubMap(ObjectManager& objm)
{
    objm.mEntm->SpawnEntityOnMap(objm, objm.mSubMap, objm.mEntm->mEntTable[0], 51);
    objm.mEntm->SpawnEntityOnMap(objm, objm.mSubMap, objm.mEntm->mPawnTable[0], 34);
    SDL_Log("entities spawned at submap");
}

void NGScenario::LoadOverMap(ObjectManager& objm)
{
}

void NGScenario::LoadCityMap(ObjectManager& objm)
{
}


void DefScenario::LoadEntities(ObjectManager& objm)
{
}

void DefScenario::LoadSubMap(ObjectManager& objm)
{
}
void DefScenario::LoadOverMap(ObjectManager& objm)
{
}
void DefScenario::LoadCityMap(ObjectManager& objm)
{
}

ScenarioManager::ScenarioManager()
{
    mCurrentSc = new DefScenario();
}

void ScenarioManager::SetCurrentScenario(Scenario *sc)
{
    DestroyCurrentScenario();
    mCurrentSc = sc;
}

void ScenarioManager::DestroyCurrentScenario()
{
    if (mCurrentSc != nullptr) delete mCurrentSc;
}

void ScenarioManager::LoadThings(ObjectManager& objm)
{
    mCurrentSc->LoadEntities(objm);
    mCurrentSc->LoadSubMap(objm);
    mCurrentSc->LoadOverMap(objm);
    mCurrentSc->LoadCityMap(objm);
}

void ScenarioManager::ClearThings(ObjectManager& objm)
{
}
