#pragma once

#include "entity/shot_data.h"
#include "entity/targetable.h"

#include <cstdint>

class Tower;
class Gem;

class TowerShot
{
private:
    float lastTargetX;
    float lastTargetY;

    float scatterX;
    float scatterY;

    float velZ;

    float z; // altitude

    float scaleBonus;

public:
    TowerShot(const Tower& tower, Targetable* pTarget_);

    bool tick(int frames);

    ShotData shot;
    double damage;
    double crit;
    bool isKillingShot;

    Gem* pSourceGem;
    Targetable* pTarget;

    float x;
    float y;

    float velX;
    float velY;

    float scale;
    float angle;

    uint32_t RGB;
};
