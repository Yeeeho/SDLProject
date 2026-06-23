#include "pch.h"

#include "city.h"
#include "system.h"
#include "texture.h"

Facility::Facility(int x, int y, int w, int h)
{
    mX = x; mY = y;
    mW = w; mH = h;

    mFacTex = new Texture();

    //초기 패스
    std::string path = "images/facility/frame.png";

    if (mFacTex->LoadFromFile(path) == false) {
        SDL_Log("could not load frame file");
    }
    SDL_SetTextureBlendMode(mFacTex->mTexture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
}

void Facility::ChangeTexture(std::string path)
{
    //텍스처 다시 로드
    if (mFacTex->LoadFromFile(path) == false) {
        SDL_Log(SDL_GetError());
    }
}

City::City()
{
}

CityMap::CityMap()
{
    TextureManager tm;
    mTempTexture = tm.CreateTempTexture();
}

void CityMap::GenerateFacs(int xTiles, int yTiles, int len)
{
    int id = 0;
    //흔한 2차원 배열 만들기
    for (int i = 0; i < yTiles; i++) {
        for (int j = 0; j < xTiles; j++) {

            Facility* fac = new Facility(j*len, i*len, len, len);
            fac->mId = id; //id 할당
            mFacs.push_back(fac);
            
            id++;
        }
    }
}

void CityMap::RenderOnUpdate()
{
    //업데이트 플래그가 거짓이면 저장된 텍스처를 렌더링한다.
    if (mIsMapUpdate == false) {
        SDL_RenderTexture(System::sRenderer , mTempTexture, nullptr, nullptr);
        return;
    }
    //업데이트가 참이면
    SDL_Log("updating cityMap on update flag");
    SDL_SetRenderTarget(System::sRenderer, mTempTexture); //렌더러 타겟으로 설정
    SDL_SetRenderDrawColor(System::sRenderer, 0x00, 0x00, 0x00, 0x00);
    SDL_SetTextureBlendMode(mTempTexture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
    SDL_RenderClear(System::sRenderer);

    //실제 렌더링
    if (mFacs.empty()) {
        SDL_Log("no facilities in the map");
    }

    for (Facility* facility : mFacs) {
        facility->mFacTex->Render(
            //시작위치를 기준으로 시설의 위치를 더함.
            mX + facility->mX, mY + facility->mY
        );
    }

    //렌더러 타겟 해제
    SDL_SetRenderTarget(System::sRenderer, NULL);
    //렌더링 완료하면 플래그 변수 초기화
    mIsMapUpdate = false;
}
