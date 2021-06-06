#pragma once

#include <climits>

class TowerShot;

class Targetable
{
public:
    Targetable()
        : x(0.0f),
          y(0.0f),
          distanceToOrb(INT_MAX),
          incomingShots(0),
          isKillingShotOnTheWay(false),
          isKilled(false)
    {
    }

    virtual void receiveShotDamage() = 0;
    virtual float getShotVariance() { return 0.0f; }
    virtual float canBeTargeted() { return true; }

    float x;
    float y;

    int distanceToOrb;

    int incomingShots;
    bool isKillingShotOnTheWay;
    bool isKilled;
};
