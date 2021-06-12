#pragma once

#include "entity/shot_data.h"
#include "entity/targetable.h"

#include <cstdint>

class Tower;
class Gem;

class TowerShot
{
public:
    TowerShot(const Tower& tower, Targetable* pTarget_);

    bool tick(int frames);

    ShotData shot;
    double damage;
    double crit;
    bool isKillingShot;

    Gem* pSourceGem;
    Targetable* pTarget;
    float lastTargetX;
    float lastTargetY;
    float scatterX;
    float scatterY;

    float x;
    float y;
    float z; // altitude

    float velX;
    float velY;
    float velZ;

    uint32_t RGB;
};
