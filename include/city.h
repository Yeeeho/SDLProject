#pragma once

#include <vector>

//도시 클래스
class City {
    public:
    City();

    int mSupply = 0;
    int mEnergy = 0;
};

//전방선언
class Facility;
//도시 맵 클래스
class CityMap {
    public:
    CityMap();

    void GenerateFacs(int xTiles, int yTiles, int len); //시설 객체를 벡터에 생성
    //렌더링 함수
    void RenderOnUpdate();

    std::vector<Facility*> mFacs; //시설 객체들을 담음
    SDL_Texture* mTempTexture{nullptr}; //베이킹할 임시 텍스처

    bool mIsMapUpdate = true; //임시 텍스처 렌더링 업데이트 플래그
    
    //맵의 시작 위치
    int mX = 300;
    int mY = 100;
};

//전방선언
class Texture;
//시설 클래스
class Facility {
    public:
    Facility(int x, int y, int w, int h); //좌표를 받는다.

    void ChangeTexture(std::string path);

    int mId = 0; //식별용 id
    int mManReq = 0; //시설에 필요한 인원

    //위치와 크기
    int mX, mY;
    int mW, mH;

    Texture* mFacTex{nullptr};
};

