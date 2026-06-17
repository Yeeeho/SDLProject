#include "pch.h"

#include "map.h"
#include "system.h"
#include "square.h"

Map::Map(int xt, int yt)
{
    mTileLen = 100;

    mXTiles = xt;
    mYTiles = yt;

    mX = System::sWindowWidth/2 - (mXTiles*mTileLen/2);
    mY = 100;

}

void Map::Render()
{
    Square mapFrame = Square(System::sWindowWidth/2 - (mXTiles*mTileLen/2), 100,  mXTiles*mTileLen, mYTiles*mTileLen);

    mapFrame.Render();
    //세로줄을 긋는다.
    for (int i = 1; i < mXTiles; i++) {
        SDL_RenderLine(System::sRenderer, mX + mTileLen*i, mY, mX + mTileLen*i, mY + mTileLen*mYTiles);
    }
    //가로줄을 긋는다.
    for (int i = 1; i < mYTiles; i++) {
        SDL_RenderLine(System::sRenderer, mX, mY + mTileLen*i , mX + mTileLen*mXTiles, mY + mTileLen*i);
    }
}
