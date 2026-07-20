#pragma once

#include <vector>
#include <string>
#include <unordered_map>

//전방 선언
class Camera;
class Entity;
class MapTile;
class Texture;
class UI;

//월드맵 클래스
class Map {
    public:
    Map(int x, int y, int xTiles, int yTiles, int tileLen);
    void GenerateMapTiles();
    void GenerateCityTiles();

    void Destroy();

    //이벤트 핸들링
    void HandleEvent(SDL_Event& e, float mouseX, float mouseY);

    //업데이트 관련
    bool mIsMapUpdate {true};

    //렌더링 관련
    void RenderOnUpdate();

    //맵 카메라
    Camera* mCam {nullptr};

    //일반적인 정보
    int mX {0}, mY {0}; //맵 위치
    int mInitX {0}, mInitY {0}; //맵 시작 위치

    int mW, mH = 0; //맵의 크기. 타일 길이 * 타일 개수
    int mXTiles, mYTiles = 0; //축의 타일 개수
    int mTileLen = 100; //타일 한 변 길이

    std::vector<MapTile*> mMapTiles; //맵타일들 담는 컨테이너
    std::vector<Entity*> mNpcs; //맵에 있는 npc들 담는 컨테이너
    std::vector<Entity*> mPawns; //맵에 있는 폰들을 담는 컨테이너

    SDL_Texture* mTempTex{nullptr}; //베이킹용 임시 텍스처
};

//전방선언
class ToolTip;
class TTFWord;
//타일 클래스
class MapTile {
    public:
    MapTile(int x, int y, int w, int h, std::string path = "images/map/frame.png");
    void Destroy();

    void ChangeTexture(std::string path);
    void DestroyInfos(); //참조 데이터 컨테이너를 비우는 함수. 

    int mId = 0; //식별용 타일 아이디

    int mX {0}, mY {0}; //위치와 크기
    int mW, mH; 

    std::vector<TTFWord*> mInfos;

    Texture* mTileTex{nullptr}; //타일 텍스처, 항상 렌더링됨.
};

class MapManager {
    public:
    int WhatTileOnPoint(float x, float y, Map* map); //이 점은 맵의 어느 타일에 있는가?

    //브레젠험 
    std::vector<int> GetTilesIdBetween(Map* map, MapTile* tile1, MapTile* tile2); //타일 두개를 이었을때 그 사이에 무슨 타일들이 있는지 구함.

    std::unordered_map<std::string, int> PosXYByTileId(int id, Map* map);
};
