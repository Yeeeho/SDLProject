#include "pch.h"

#include "game_object.h"
#include "text.h"
#include "city.h"
#include "entity.h"
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

void ObjectManager::InitStartObjects()
{
    //오버맵에 도시 생성
    int cityIdx = mMap->mXTiles * mMap->mYTiles * 0.5 - mMap->mXTiles * 0.5;
    mMap->mMapTiles[cityIdx]->ChangeTexture("images/map/city.png");

    SDL_Color tc = {0xE0, 0xE0, 0xE0, 0xFF};
    mMap->mMapTiles[cityIdx]->mInfos.push_back(new TTFWord("당신의 도시", tc, System::sFont));
    mMap->mMapTiles[cityIdx]->mInfos.push_back(new TTFWord(System::sFont, TextType::NewLine));

    //도시 맵 초기화

    //오버맵에 팀 생성
    mTeamm->AllocTeamOnTable("slaves", "images/entity/team/enemy_team_red.png", 0);
    Team* targetTeam = mTeamm->mTeamTable[0];
    mEntm->AllocEntityOnTable(*this, "slave", 0);
    Entity* slave = mEntm->mEntTable[0];

    mTeamm->SpawnTeamOnMap(mMap, targetTeam, cityIdx + 1);
    mTeamm->PutEntInTeam(targetTeam, slave);

    mTeamm->AllocPTeamOnTable("당신의 팀", 0);
    mEntm->AllocPawnOnTable(*this, "", PawnType::Unique, 0);
}
