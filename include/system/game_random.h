#pragma once

#include <random>

class Random {
    public:
    static bool BDistribution(float num);
    static int IDistribution(int min, int max);
    static float fDistribution(float min, float max);

    private:
    inline static std::default_random_engine dre;
};