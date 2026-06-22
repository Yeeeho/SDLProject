#pragma once

#include <vector>

class City {
    public:
    City();

    int mSupply = 0;
    int mEnergy = 0;
};

class Facility;
class CityMap {
    public:
    CityMap();

    std::vector<Facility*> facs;
    
    int mW = 3;
    int mH = 3;
};

class Facility {
    public:
    Facility(int x, int y); //좌표를 받는다.
    
    int manReq = 0; //시설에 필요한 인원
    int mX, mY;
    int mW = 1;
    int mH = 1;
};

