#include "pch.h"

#include "system.h"
#include "city.h"
#include "map.h"

City::City(int x, int y)
{
    mCityMap = new Map(System::sWindowWidth*0.5 - 32*25, y, 32, 32, 50);
    mCityMap->GenerateCityTiles();
}
