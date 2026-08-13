#include "pch.h"

#include "render.h"
#include "game_context.h"
#include "game_object.h"
#include "turn.h"
#include "map.h"
#include "ui.h"
#include "math.h"
#include "camera.h"
#include "text.h"
#include "texture.h"
#include "system.h"
#include "square.h"
#include "entity.h"
#include "skill/skill.h"
#include "move.h"

Grid::Grid(int x, int y, int offsetX, int offsetY, int xTiles, int yTiles, int tileLen, GridType gt)
{
    mGridType = gt;
    mTileLen = tileLen;
    mXTiles = xTiles; mYTiles = yTiles;

    mW = mTileLen*mXTiles;
    mH = mTileLen*mYTiles;

    mX = x; mY = y;
    mOffsetX = offsetX; mOffsetY = offsetY;
    mInitX = x; mInitY = y;

    mCam = new Camera(0, 0, 0);
}

void Grid::RenderTiles()
{
    Texture t;
    t.LoadFromFile("images/ui/frame.png");
    SDL_SetTextureScaleMode(t.mTexture, SDL_SCALEMODE_NEAREST);

    for (int i = 0; i < mYTiles; i++) {
        for (int j = 0; j < mXTiles; j++) {
            t.Render((float) mOffsetX + j*mTileLen,(float) mOffsetY + i*mTileLen, nullptr,(float) mTileLen,(float) mTileLen);
        }
    }
}

Map::Map(int x, int y, int offsetX, int offsetY, int xTiles, int yTiles, int tileLen)
{
    mGridType = GridType::Map;
    mTileLen = tileLen;
    mXTiles = xTiles; mYTiles = yTiles;

    mW = mTileLen*mXTiles;
    mH = mTileLen*mYTiles;

    mX = x; mY = y;
    mOffsetX = offsetX; mOffsetY = offsetY;
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
    mCurrentTurn = 0;

    delete this;
}

void Map::HandleEvent(SDL_Event &e, GameContext& gc, float mouseX, float mouseY)
{
    if(gc.mUim->mDialogueUI->mIsRender) return; //대화창이 렌더링중이면 반환한다.
    if(!mCanHandleEvent) return;

    //마우스오버
    //맵 이벤트 핸들러는 맵상에서의 범위 표시 등을 제어한다.
    //엔티티 자체의 포커스 여부 등은 제어하지 않는다.

    MapHelper mh;
    mouseX += mCam->mSight.x; //카메라 보정
    mouseY += mCam->mSight.y;

    Math mth;
    bool isInMap = mth.IsPointInSquare(mouseX, mouseY, mOffsetX, mOffsetY, mW, mH); 
    if (!isInMap) { //맵 밖에 있는 경우
        gc.mUim->mTileHLUI->mIsRenderBetweenTiles = false; //타일 ui 렌더링 플래그 거짓
        return;
    }

    int tid = mh.WhatTileOnPoint(mouseX, mouseY, this);
    MapTile* tile = mMapTiles[tid];

    //턴을 잡은 엔티티가 있을 경우 타겟을 금마로 설정
    Entity* target = gc.mTurnm->mCurrentTarget;
    if (!target) return; //턴을 잡은 엔티티가 없으면 반환

    //아군 엔티티 이동 동작 
    //타겟이 포커스 상태고 아군일때 이동 타일 범위 렌더링
    if (target != gc.mObjm->mEntm->mFocusedEnt || !target->mIsPawn) {
        //둘중 하나라도 만족하지 않으면 렌더링 안함
        gc.mUim->mTileHLUI->mIsRenderBetweenTiles = false;
        return;
    }
    else {
        gc.mUim->mTileHLUI->mIsRenderBetweenTiles = true;
    }

    //타일 범위 구하는 로직
    //두 타일 사이의 아이디가 담긴 컨테이너를 구한다.
    MapTile* tile1 = mMapTiles[target->mTileId];
    MapTile* tile2 = tile;
    std::vector<int> tids = mh.GetTilesIdBetween(this, tile1, tile2);
    
    //타일 ui 관련 세팅
    if (gc.mSkm->mIsSkillReady) { //스킬매니저가 스킬을 사용할 준비가 되었다면
        //범위에 따라 타일을 제한한다.
        //현재는 광역공격을 위한 계산식은 없다.
        SkillHelper skh;
        int range = skh.GetSkillRange(gc.mSkm->mSkillData, gc.mSkm->mSkill);
        
        if (range + 1 > tids.size()) {            
            //강조될 타일 개수보다 스킬 범위가 크면 타일 컨테이너를 조정하지 않는다.
        } 
        else {
            //스킬 범위가 타일 개수보다 적은 경우
            std::vector<int> tidsCpy = tids; //복사해놓는다.
            tids.clear(); //실제 사용할 객체를 비운다.
            for (int i = 0; i < range+1; i++) {
                tids.push_back(tidsCpy[i]); //복사된 친구에게서 다시 원소를 받는다. 범위만큼만.
            }
        }

        gc.mUim->mTileHLUI->SetTileIds(tids);
        //마우스 왼쪽 클릭하고 놓았을 때
        if (!System::sIsLeftMouseClicked || e.type != SDL_EVENT_MOUSE_BUTTON_UP) return;

        //기술 발현데스
        gc.mSkm->mMap = this; //이 단계에서 스킬 발동에 필수적인 멤버 변수들이 모두 설정된다.
        gc.mSkm->SetTileIds(tids);
        gc.mSkm->ActivateSkill();
    }
}

void Map::GenerateMapTiles()
{
    int id = 0;

    for (int i = 0; i < mYTiles; i++) {
        for (int j = 0; j < mXTiles; j++) {
            MapTile* tile = new MapTile(mOffsetX + j*mTileLen, mOffsetY + i*mTileLen, mTileLen, mTileLen);
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
            MapTile* tile = new MapTile(mOffsetX + j*mTileLen, mOffsetY + i*mTileLen, mTileLen, mTileLen, path);
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

int MapHelper::WhatTileOnPoint(float x, float y, Grid *grid)
{
    float xDis = x - static_cast<float>(grid->mOffsetX + grid->mX);
    float yDis = y - static_cast<float>(grid->mOffsetY + grid->mY);
    int xPos = xDis/grid->mTileLen; //상대 거리를 타일 크기로 나누어 타일 좌표를 구한다.
    int yPos = yDis/grid->mTileLen;

    //좌표에 한계를 지정한다.
    if (xPos >= grid->mXTiles) xPos = grid->mXTiles - 1;
    if (yPos >= grid->mYTiles) yPos = grid->mYTiles - 1;
    if (xPos < 0) xPos = 0;
    if (yPos < 0) yPos = 0;

    //맵 아이디를 구한다.
    int id = xPos + (grid->mXTiles * yPos);
    return id;
}

std::vector<int> MapHelper::GetTilesIdBetween(Map *map, MapTile *tile1, MapTile *tile2)
{
    std::vector<int> ret;

    int tileLen = map->mTileLen;
    int x1 = (tile1->mX - map->mOffsetX) / tileLen;
    int y1 = (tile1->mY - map->mOffsetY) / tileLen;
    int x2 = (tile2->mX - map->mOffsetX) / tileLen;
    int y2 = (tile2->mY - map->mOffsetY) / tileLen;

    //여기서 브레젠험 알고리즘을 사용한다.
    int dx = std::abs(x2 - x1);
    int dy = -std::abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1; 
    int sy = y1 < y2 ? 1 : -1;

    int err = dx + dy; int e2; //오차.

    while (true) {
        int id = WhatTileOnPoint(x1 * tileLen + map->mOffsetX, y1 * tileLen + map->mOffsetY, map);
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

std::unordered_map<std::string, int> MapHelper::PosXYByTileId(int id, Grid *grid)
{
    int posY = id / grid->mXTiles;
    int posX = id - posY * grid->mXTiles;

    std::unordered_map<std::string, int> ret;
    ret.insert({"x", posX});
    ret.insert({"y", posY});

    return ret;
}

MapManager::MapManager()
{
    //오버맵에 도시 생성
    mOverMap = new Map(0, 0, System::sWindowWidth/2 - 6 * 50 , 100, 6 ,6, 100); //월드 맵 객체 생성
    mOverMap->GenerateMapTiles();
    
    int cityIdx = mOverMap->mXTiles * mOverMap->mYTiles * 0.5 - mOverMap->mXTiles * 0.5;
    mOverMap->mMapTiles[cityIdx]->ChangeTexture("images/map/city.png");

    SDL_Color tc = {0xE0, 0xE0, 0xE0, 0xFF};

    mSubMap = new Map(0, 0, System::sWindowWidth*0.5 - 6*40, 100, 16, 16, 80); //서브맵 객체 생성
    mSubMap->GenerateMapTiles();

    mCityMap = new Map(0, 0, System::sWindowWidth*0.5 - 6*40, 100, 32, 32, 80);
    mCityMap->GenerateCityTiles();
}

