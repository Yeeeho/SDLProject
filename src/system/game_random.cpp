#include "pch.h"

#include "system/game_random.h"

bool Random::BDistribution(float num)
{
    if (num > 1.f || num < 0.f) {
        SDL_Log("[WARNING] bDistribution: number must be between 1 and 0");
        return false;
    }

    std::bernoulli_distribution bDist(num);
    return bDist(dre);
}

int Random::IDistribution(int min, int max)
{
    std::uniform_int_distribution<int> iDIst(min, max);
    return iDIst(dre);
}

float Random::fDistribution(float min, float max)
{
    std::uniform_real_distribution<float> fDist(min, max);
    return fDist(dre);
}
