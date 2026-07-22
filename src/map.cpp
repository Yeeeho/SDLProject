#include "pch.h"

#include "map.h"
#include "move.h"
#include "game_object.h"
#include "ui.h"
#include "entity.h"
#include "math.h"
#include "camera.h"
#include "text.h"
#include "texture.h"
#include "system.h"
#include "square.h"

Map::Map(int x, int y, int xTiles, int yTiles, int tileLen)
{
    mTileLen = tileLen;
    mXTiles = xTiles; mYTiles = yTiles;

    mW = mTileLen*mXTiles;
    mH = mTileLen*mYTiles;

    mX = x; mY = y;
    mInitX = x; mInitY = y;

    mCam = new Camera(10, System::sWindowWidth + 4000, System::sWindowHeight + 4000);

    TextureManager tm;
    mTempTex = tm.CreateTempTexture(
        System::sRenderer,
        System::sWindowWidth + 4000, System::sWindowHeight + 4000
    );
}

void Map::Destroy()
{
    if (mTempTex != nullptr) {
        SDL_DestroyTexture(mTempTex);
        mTempTex = nullptr;
    }

    for (MapTile* tile : mMapTiles) {
        tile->Destroy();
        tile = nullptr;
    }

    mMapTiles.clear();

    delete this;
}

void Map::HandleEvent(SDL_Event &e, UIManager& uim, ObjectManager& objm, float mouseX, float mouseY)
{
    if(uim.mDialogueUI->mIsRender) return; //대화창이 렌더링중이면 반환한다.  
    //마우스오버
    //맵 이벤트 핸들러는 맵상에서의 범위 표시 등을 제어한다.
    //엔티티 자체의 포커스 여부 등은 제어하지 않는다.

    MapManager mm;
    mouseX += mCam->mSight.x; //카메라 보정
    mouseY += mCam->mSight.y;

    Math mth;
    bool isInMap = mth.IsPointInSquare(mouseX, mouseY, mX, mY, mW, mH); 
    if (!isInMap) { //맵 밖에 있는 경우
        uim.mTileHLUI->mIsRenderBetweenTiles = false; //타일 ui 렌더링 플래그 거짓
        return;
    }

    int tid = mm.WhatTileOnPoint(mouseX, mouseY, this);
    MapTile* tile = mMapTiles[tid];

    Entity* target {nullptr};
    //턴을 잡은 아군이 있을 경우 타겟을 금마로 설정
    for (Entity* ent : mPawns) {
        if (ent->mIsTakingTurn) {
            target = ent;
        }
    }    
    if (!target) return; //턴을 잡은 아군이 없으면 반환

    //아군 엔티티 이동 동작 
    //타겟이 포커스 상태고 아군일때 이동 타일 범위 렌더링
    if (target != mFocusedEnt || !target->mIsPawn) {
        //둘중 하나라도 만족하지 않으면 렌더링 안함
        uim.mTileHLUI->mIsRenderBetweenTiles = false;
        return;
    }
    else {
        uim.mTileHLUI->mIsRenderBetweenTiles = true;
    }

    //타일 범위 구하는 로직
    if (target->mTileId == tid) { 
        return; //같은 타일을 두번 누름
    }
    MapTile* tile1 = mMapTiles[target->mTileId];
    MapTile* tile2 = tile;
    //TODO:이동 능력에 따라서 조절되어야 함.
    std::vector<int> tids = mm.GetTilesIdBetween(this, tile1, tile2);
    
    //타일 ui 관련 세팅
    uim.mTileHLUI->SetTileIds(tids);
    
    if (tile->mIsEntOn) {
        //목표 타일에 엔티티가 이미 있음
        return;        
    }
    //마우스 왼쪽 클릭시 행동
    if (e.type != SDL_EVENT_MOUSE_BUTTON_DOWN) return;
    if (e.button.button != SDL_BUTTON_LEFT) return; 
    //엔티티 이동
    MoveManager mvm = MoveManager(&uim, &objm);
    mvm.MoveEntityTo(this, target, target->mTileId, tid);
}

void Map::GenerateMapTiles()
{
    int id = 0;

    for (int i = 0; i < mYTiles; i++) {
        for (int j = 0; j < mXTiles; j++) {
            MapTile* tile = new MapTile(mX + j*mTileLen, mY + i*mTileLen, mTileLen, mTileLen);
            tile->mId = id;

            mMapTiles.push_back(tile);

            id++;
        }
    }
}

void Map::GenerateCityTiles()
{
    int id = 0;
    std::string path = "images/facility/frame.png";

    for (int i = 0; i < mYTiles; i++) {
        for (int j = 0; j < mXTiles; j++) {
            MapTile* tile = new MapTile(mX + j*mTileLen, mY + i*mTileLen, mTileLen, mTileLen, path);
            tile->mId = id;

            mMapTiles.push_back(tile);

            id++;
        }
    }
}

void Map::RenderOnUpdate()
{
    if (mIsMapUpdate == false) {
        SDL_RenderTexture(System::sRenderer, mTempTex, &mCam->mSight, nullptr);
        return;
    }

    SDL_Log("updating Map on update flag");
    SDL_SetRenderTarget(System::sRenderer, mTempTex); //렌더러 타겟으로 설정
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_SetTextureBlendMode(mTempTex, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
    SDL_RenderClear(System::sRenderer);

    for (MapTile* tile : mMapTiles) {
        tile->mTileTex->Render(
            tile->mX, tile->mY, nullptr,
            static_cast<float>(mTileLen), static_cast<float>(mTileLen) 
        );
    }

    //렌더러 타겟 해제
    SDL_SetRenderTarget(System::sRenderer, NULL);
    //렌더링 완료하면 플래그 변수 초기화
    mIsMapUpdate = false;
}

MapTile::MapTile(int x, int y, int w, int h, std::string path)
{
    mX = x; mY = y;
    mW = w; mH = h;

    mIsEntOn = false;

    mTileTex = new Texture();

    if (mTileTex->LoadFromFile(path) == false) {
        std::string message = path + " not loaded";
        SDL_Log(message.c_str());
    }
    SDL_SetTextureBlendMode(mTileTex->mTexture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
    SDL_SetTextureScaleMode(mTileTex->mTexture, SDL_SCALEMODE_NEAREST);
}

void MapTile::Destroy()
{
    if (mTileTex != nullptr) {
        mTileTex->Destroy();
    }

    delete this;
}

void MapTile::ChangeTexture(std::string path)
{
    if (mTileTex->LoadFromFile(path) == false) {
        SDL_Log(SDL_GetError());
    }
}

void MapTile::DestroyInfos()
{
    for (TTFWord* word : mInfos) {
        word->Destroy();
    }

    mInfos.clear();
}

int MapManager::WhatTileOnPoint(float x, float y, Map *map)
{
    float xDis = x - static_cast<float>(map->mX);
    float yDis = y - static_cast<float>(map->mY);
    int xPos = xDis/map->mTileLen; //상대 거리를 타일 크기로 나누어 타일 좌표를 구한다.
    int yPos = yDis/map->mTileLen;

    //좌표에 한계를 지정한다.
    if (xPos >= map->mXTiles) xPos = map->mXTiles - 1;
    if (yPos >= map->mYTiles) yPos = map->mYTiles - 1;
    if (xPos < 0) xPos = 0;
    if (yPos < 0) yPos = 0;

    //맵 아이디를 구한다.
    int id = xPos + (map->mXTiles * yPos);
    return id;
}

std::vector<int> MapManager::GetTilesIdBetween(Map *map, MapTile *tile1, MapTile *tile2)
{
    std::vector<int> ret;

    int tileLen = map->mTileLen;
    int x1 = (tile1->mX - map->mX) / tileLen;
    int y1 = (tile1->mY - map->mY) / tileLen;
    int x2 = (tile2->mX - map->mX) / tileLen;
    int y2 = (tile2->mY - map->mY) / tileLen;

    //여기서 브레젠험 알고리즘을 사용한다.
    int dx = std::abs(x2 - x1);
    int dy = -std::abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1; 
    int sy = y1 < y2 ? 1 : -1;

    int err = dx + dy; int e2; //오차.

    while (true) {
        int id = WhatTileOnPoint(x1 * tileLen + map->mX, y1 * tileLen + map->mY, map);
        ret.push_back(id);

        e2 = 2*err;
        if (e2 >= dy) {
            if (x1 == x2) break;
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            if (y1 == y2) break;
            err += dx;
            y1 += sy;
        }
    }
    return ret;
}   

std::unordered_map<std::string, int> MapManager::PosXYByTileId(int id, Map *map)
{
    int posY = id / map->mXTiles;
    int posX = id - posY * map->mXTiles;

    std::unordered_map<std::string, int> ret;
    ret.insert({"x", posX});
    ret.insert({"y", posY});

    return ret;
}
