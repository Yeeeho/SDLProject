#pragma once

#include <unordered_map> 

class Map;

class Facility {
    public:
    Facility() = default;

    int mId;
};

class City {
    public:
    City(int x, int y);

    std::unordered_map<int, Facility*> mFacs;

    Map* mCityMap {nullptr};
};

