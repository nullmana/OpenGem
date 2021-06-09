#pragma once

#include "constants/gem_component_type.h"

#include "entity/shot_data.h"

#include <cstddef>
#include <cstdint>

class Building;

class Gem
{
public:
    Gem(int grade_, GEM_COMPONENT_TYPE type);
    Gem(Gem* pSourceGem); // Duplicate Gem Constructor, not Copy

    static double gemCreateCostCurrent;
    static double gemCombineCostCurrent;

    Building* pBuilding;
    float x;
    float y;
    bool isDragged;

    uint32_t HSV;
    uint32_t RGB;

    int grade;
    double manaCost;

    ShotData shotRaw;
    ShotData shotAmplified;
    ShotData shotFinal;

    // Recalculate own ShotData based on combining pOther into this gem
    void combineWith(const Gem* pOther);
    void recalculateShotData();

    static double gemCreateCost(int grade);

#ifdef DEBUG
    void debugPrint();
#endif
};
