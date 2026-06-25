#include "pch.h"

#include "map.h"
#include "texture.h"
#include "system.h"
#include "square.h"

Map::Map(int xt, int yt)
{
    mTileLen = 100;
    mXTiles = xt; mYTiles = yt;

    mX = System::sWindowWidth/2 - (mXTiles*mTileLen/2);
    mY = 100;

    TextureManager tm;
    mTempTex = tm.CreateTempTexture();
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

void Map::

RenderOnUpdate()
{
    if (mIsMapUpdate == false) {
        SDL_RenderTexture(System::sRenderer , mTempTex, nullptr, nullptr);
        return;
    }

    SDL_Log("updating Map on update flag");
    SDL_SetRenderTarget(System::sRenderer, mTempTex); //렌더러 타겟으로 설정
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_SetTextureBlendMode(mTempTex, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
    SDL_RenderClear(System::sRenderer);

    for (MapTile* tile : mMapTiles) {
        tile->mTileTex->Render(
            tile->mX, tile->mY
        );
    }

    //렌더러 타겟 해제
    SDL_SetRenderTarget(System::sRenderer, NULL);
    //렌더링 완료하면 플래그 변수 초기화
    mIsMapUpdate = false;
}

MapTile::MapTile(int x, int y, int w, int h)
{
    mX = x; mY = y;
    mW = w; mH = h;
    mTileTex = new Texture();

    std::string path = "images/map/frame.png";
    if (mTileTex->LoadFromFile(path) == false) {
        std::string message = path + " not loaded";
        SDL_Log(message.c_str());
    }
    SDL_SetTextureBlendMode(mTileTex->mTexture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
}

void MapTile::ChangeTexture(std::string path)
{
    if (mTileTex->LoadFromFile(path) == false) {
        SDL_Log(SDL_GetError());
    }
}
