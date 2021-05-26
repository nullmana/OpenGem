#pragma once

#include "constants/building_type.h"

class Building
{
public:
    Building(int ix_, int iy_) : ix(ix_), iy(iy_), x(ix_ + 1.0f), y(iy_ + 1.0f) {}
    BUILDING_TYPE type;

    int ix;
    int iy;

    float x;
    float y;
};
