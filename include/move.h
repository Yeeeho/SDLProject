#pragma once

#include <queue>

class UIManager;
class ObjectManager;
class Entity;
class Map;
class MapTile;

class MoveManager {
    public:
    MoveManager(UIManager* uim, ObjectManager* objm);
    UIManager* mUim {nullptr}; //해제하면 큰일난다.
    ObjectManager* mObjm {nullptr};

    void SetEntitySpeed(Entity* ent, int xspd, int yspd);
    void MoveEntity(Map* map, Entity* ent);
    void MoveEntityTo(Map* map, Entity* ent, MapTile* tile1, MapTile* tile2);

    void MoveEntityTo(Map* map, Entity* ent, int currentTileId, int targetTileId);

    Uint64 mMs {0};
    Uint64 mMaxFrameCapMs {500};
};

class MoveHelper {
    public:
    bool CheckDiagonalMove(int firstTileId, int lastTileId, Map* map);
    //대각선 이동을 순서대로 큐에 집어넣음
    std::queue<bool> GetDiagonalMoveQueue(std::vector<int> tids, Map* map);
    //대각선 이동이 총 몇번 일어나는지만 구함
    int GetDiagonalMoves(std::vector<int> tids, Map* map);

    int GetApCost(std::vector<int> tids, Map* map, int apPerTile);
};