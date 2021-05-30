#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>

class Building;

class Gem
{
public:
    Gem(int grade_, uint32_t color_ = 0xAABBCC);
    Gem(Gem* pSourceGem); // Duplicate Gem Constructor, not Copy

    static double gemCreateCostCurrent;
    static double gemCombineCostCurrent;

    Building* pBuilding;
    float x;
    float y;
    bool isDragged;

    uint32_t color;

    int grade;
    double manaCost;

    static double gemCreateCost(int grade);
};
