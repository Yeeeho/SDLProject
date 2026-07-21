#pragma once

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