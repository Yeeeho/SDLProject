#include "pch.h"

#include "map.h"
#include "physics.h"
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

//최적화/기능 추가 여지가 남아있음.
void Map::HandleEvent(SDL_Event &e, float mouseX, float mouseY)
{

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
std::unordered_map<std::string, int> MapManager::PosXYByTileId(int id, Map *map)
{
    int posY = id / map->mXTiles;
    int posX = id - posY * map->mXTiles;

    std::unordered_map<std::string, int> ret;
    ret.insert({"x", posX});
    ret.insert({"y", posY});

    return ret;
}
