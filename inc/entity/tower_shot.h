#pragma once

#include "entity/targetable.h"

#include <cstdint>

class Tower;

class TowerShot
{
public:
    TowerShot(const Tower& tower, Targetable* pTarget_);

    bool tick(int frames);

    Targetable* pTarget;
    float lastTargetX;
    float lastTargetY;

    float x;
    float y;
    float z; // altitude

    float velX;
    float velY;
    float velZ;

    uint32_t RGB;
};
