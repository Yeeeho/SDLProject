#include "pch.h"

#include "city.h"
#include "system.h"
#include "texture.h"

Facility::Facility(int x, int y, int w, int h)
{
    mX = x; mY = y;
    mW = w; mH = h;

    mFacTex = new Texture();

    std::string path = "images/facility/frame.png";
    if (mFacTex->LoadFromFile(path) == false) {
        SDL_Log("could not load frame file");
    }
    SDL_SetTextureBlendMode(mFacTex->mTexture, SDL_BLENDMODE_BLEND_PREMULTIPLIED);
}

City::City()
{
}

CityMap::CityMap()
{
    TextureManager tm;
    mTempTexture = tm.CreateTempTexture();
}

void CityMap::GenerateFacs()
{
    Facility* fac = new Facility(0, 0, 100, 100);
    facs.push_back(fac);
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
    if (facs.empty()) {
        SDL_Log("city map empty");
    }

    for (Facility* facility : facs) {
        facility->mFacTex->Render(
            300 + facility->mX, 300 + facility->mY
        );
    }

    //렌더러 타겟 해제
    SDL_SetRenderTarget(System::sRenderer, NULL);
    //렌더링 완료하면 플래그 변수 초기화
    mIsMapUpdate = false;
}
