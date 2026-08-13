#pragma once

#include <vector>
#include <string>
#include <unordered_map>

//전방 선언
struct GameContext;
class Item; struct ItemStack;
class GameStateManager;
class UIManager;
class ObjectManager;
class Camera;
class Entity;
class MapTile;
class Texture;
class UI;

enum class GridType {
    Default, Map, Inventory
};

class Grid {
    //맵의 상위 객체
    //그리드가 필요한 곳에 써야함
    public:
    Grid() = default;
    Grid(int x, int y, int offsetX, int offsetY, int xTiles, int yTiles, int tileLen, GridType gt);

    virtual void RenderTiles();
    
    Camera* mCam {nullptr}; //카메라

    GridType mGridType {GridType::Default};

    //좌표와 크기
    int mX {0}, mY {0}; //맵 기준점
    int mOffsetX {0}, mOffsetY {0}; //기준점에서 오프셋

    int mInitX {0}, mInitY {0}; //맵 시작 위치
    
    int mW {0}, mH {0}; //맵의 크기. 타일 길이 * 타일 개수
    int mXTiles {0}, mYTiles {0}; //축의 타일 개수
    int mTileLen = 100; //타일 한 변 길이
    
    bool mIsRender {false};
    bool mIsRenderUpdate {false};
};

//월드맵 클래스
class Map : public Grid {
    public:
    Map(int x, int y, int offsetX, int offsetY, int xTiles, int yTiles, int tileLen);
    void GenerateMapTiles();
    void GenerateCityTiles();
    
    void Destroy();
    
    //이벤트 핸들링
    void HandleEvent(SDL_Event& e, GameContext& gc, float mouseX, float mouseY);
    bool mCanHandleEvent {true};
    
    //업데이트 관련
    bool mIsMapUpdate {true};
    
    //렌더링 관련
    void RenderOnUpdate();
    
    SDL_Texture* mTempTex {nullptr}; //베이킹용 임시 텍스처
        
    //턴 제어
    int mCurrentTurn {0};
    
    std::vector<MapTile*> mMapTiles; //맵타일들 담는 컨테이너

    //엔티티
    //아니 분리하지마 코드 더 더러워짐
    std::vector<Entity*> mNpcs; //맵에 있는 npc들 담는 컨테이너
    std::vector<Entity*> mPawns; //맵에 있는 폰들을 담는 컨테이너

    //맵에 스폰된 아이템들을 스택 형식으로 관리하는 응애
    std::map<int, ItemStack*> mItemStackMap; //int: 타일 아이디

    //지형지물(추가 예정)
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

    int mId = 0; //식별용 타일 아이디

    int mX {0}, mY {0}; //위치와 크기
    int mW, mH; 

    bool mIsEntOn : 1; //엔티티가 있는가?

    Texture* mTileTex{nullptr}; //타일 텍스처, 항상 렌더링됨.
};

class MapManager {
    public:
    MapManager();

    //오버맵에서 사용할 월드맵 객체다.
    Map* mOverMap {nullptr};
    //탐험/전투가 일어나는 맵이다.
    Map* mSubMap {nullptr};
    //도시의 맵이다.
    Map* mCityMap {nullptr};

    Map* mCurrentMap {nullptr};
};

class MapHelper {
    public:
    int WhatTileOnPoint(float x, float y, Grid* grid); //이 점은 맵의 어느 타일에 있는가?

    //브레젠험 
    std::vector<int> GetTilesIdBetween(Map* map, MapTile* tile1, MapTile* tile2); //타일 두개를 이었을때 그 사이에 무슨 타일들이 있는지 구함.

    std::unordered_map<std::string, int> PosXYByTileId(int id, Grid* grid);
};