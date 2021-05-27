#pragma once

#include "constants/building_type.h"

#include "entity/amplifiable.h"

#include <cstddef>
#include <list>

class Gem;

class Building : public Amplifiable
{
public:
    Building(int ix_, int iy_);
    BUILDING_TYPE type;

    Gem* pGem;

    float x;
    float y;
};
