#pragma once

#include <vector>

//전방 선언
class MapTile;
class Texture;
class UI;

//월드맵 클래스
class Map {
    public:
    Map(int xTiles, int yTiles);
    
    void GenerateMapTiles();
    void RenderOnUpdate();

    bool mIsMapUpdate = false;

    int mXTiles, mYTiles = 0; //축의 타일 개수
    int mX, mY = 0; //맵 시작 위치
    int mTileLen = 100; //타일 한 변 길이

    std::vector<MapTile*> mMapTiles; //맵타일들 담는 컨테이너
    SDL_Texture* mTempTex{nullptr}; //베이킹용 임시 텍스처
};

//전방선언
class ToolTip;
//타일 클래스
class MapTile {
    public:
    MapTile(int x, int y, int w, int h);
    
    void ChangeTexture(std::string path);

    int mId = 0; //식별용 타일 아이디

    int mX, mY; //위치와 크기
    int mW, mH; 

    Texture* mTileTex{nullptr}; //타일 텍스처, 항상 렌더링됨.
};

