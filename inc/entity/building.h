#pragma once

#include "constants/building_type.h"

#include <cstddef>

class Gem;

class Building
{
public:
    Building(int ix_, int iy_);
    BUILDING_TYPE type;

    Gem* pGem;

    int ix;
    int iy;

    float x;
    float y;
};
