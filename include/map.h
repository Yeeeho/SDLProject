#pragma once

#include <vector>
#include <string>
#include <unordered_map>

//전방 선언
class MapTile;
class Texture;
class UI;

//월드맵 클래스
class Map {
    public:
    Map(int xTiles, int yTiles);
    
    void GenerateMapTiles();
    void GenerateCityTiles();
    void RenderOnUpdate();

    bool mIsMapUpdate = false;

    int mX, mY = 0; //맵 시작 위치
    int mW, mH = 0; //맵의 크기. 타일 길이 * 타일 개수
    int mXTiles, mYTiles = 0; //축의 타일 개수
    int mTileLen = 100; //타일 한 변 길이

    std::vector<MapTile*> mMapTiles; //맵타일들 담는 컨테이너
    SDL_Texture* mTempTex{nullptr}; //베이킹용 임시 텍스처
};

//전방선언
class ToolTip;
class TTFWord;
//타일 클래스
class MapTile {
    public:
    MapTile(int x, int y, int w, int h, std::string path = "images/map/frame.png");
    
    void ChangeTexture(std::string path);
    void DestroyInfos(); //참조 데이터 컨테이너를 비우는 함수. 

    int mId = 0; //식별용 타일 아이디

    int mX, mY; //위치와 크기
    int mW, mH; 

    std::vector<TTFWord*> mInfos;

    Texture* mTileTex{nullptr}; //타일 텍스처, 항상 렌더링됨.
};

class MapManager {
    public:
    int WhatTileOnPoint(float x, float y, Map* map);
    std::unordered_map<std::string, int> PosXYByTileId(int id, Map* map);
};
