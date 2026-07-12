#include "pch.h"

#include "city.h"
#include "map.h"

City::City(int x, int y)
{
    mCityMap = new Map(x, y);
    mCityMap->GenerateCityTiles();
}
