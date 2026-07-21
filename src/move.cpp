#include "pch.h"

#include "game_object.h"
#include "move.h"
#include "ui.h"
#include "map.h"
#include "entity.h"

MoveManager::MoveManager(UIManager* uim, ObjectManager* objm)
{
    mUim = uim; mObjm = objm;
}

void MoveManager::SetEntitySpeed(Entity *ent, int xspd, int yspd)
{
    ent->mXspeed = xspd;
    ent->mYspeed = yspd;
}

void MoveManager::MoveEntity(Map* map, Entity* ent) {
    ent->mMapX += ent->mXspeed;
    ent->mXspeed = 0;

    ent->mMapY += ent->mYspeed;
    ent->mYspeed = 0;
}

void MoveManager::MoveEntityTo(Map *map, Entity *ent, MapTile *tile1, MapTile *tile2)
{
    MapManager mm;
    std::vector<int> ids;
    ids = mm.GetTilesIdBetween(map, tile1, tile2);
    if (ids.size() < 2) return; //제자리
 
    //TODO: 이 반복문 하자있으니 고쳐야된다
    for (int i = 0; i < ids.size(); i++) {
        std::unordered_map<std::string, int> xy1 = mm.PosXYByTileId(ids[i], map);
        std::unordered_map<std::string, int> xy2 = mm.PosXYByTileId(ids[i+1], map);
        int xDiff = xy2["x"] - xy1["x"];
        int yDiff = xy2["y"] - xy2["y"];

        if (xDiff != 0 && yDiff != 0) {
            //대각선 이동, 이걸 조건문으로 필터링하는 이유는 대각선일땐 이동력을 더 소모해야 하기 때문.
        }

        SetEntitySpeed(ent ,xDiff, yDiff);
        MoveEntity(map, ent);
    }
}

//개간단한 함수다.
void MoveManager::MoveEntityTo(Map *map, Entity *ent, int currentTId, int targetTId)
{
    MapTile* originTile = map->mMapTiles[currentTId];
    MapTile* targetTile = map->mMapTiles[targetTId];
    //좌표 변경
    ent->mMapX = targetTile->mX;
    ent->mMapY = targetTile->mY;

    //엔티티가 참조하는 타일 id 덮어쓰기
    ent->mTileId = targetTId;

    //구 타일의 정보 삭제후 신 타일에 정보 로드
    originTile->DestroyInfos();
    mObjm->mEntm->LoadDataInTile(targetTile, ent);
    //툴팁도 업데이트
    mUim->mToolTip->mIsUIUpdate = true;
}
