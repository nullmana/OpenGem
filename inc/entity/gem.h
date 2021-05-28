#pragma once

#include <cstddef>
#include <cstdint>

class Building;

class Gem
{
public:
    Gem(int grade_, uint32_t color_ = 0xAABBCC)
    {
        pBuilding = NULL;
        x = 0.0f;
        y = 0.0f;
        isDragged = false;

        color = color_;

        grade = grade_;
    }

    Building* pBuilding;
    float x;
    float y;
    bool isDragged;
    uint32_t color;

    int grade;
};
