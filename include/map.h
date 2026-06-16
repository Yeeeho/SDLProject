#pragma once

class Map {
    public:
    Map(int xTiles, int yTiles);
    
    void Render();

    int mXTiles, mYTiles;
    int mX, mY;
    
    int mTileLen;

};

