#pragma once

#include <climits>

class TowerShot;
class ShotData;
class Gem;

class Targetable
{
public:
    Targetable(double hp_)
    {
        x = 0.0f;
        y = 0.0f;
        distanceToOrb = INT_MAX;
        incomingShots = 0;
        incomingDamage = 0.0;
        isKillingShotOnTheWay = false;
        isKilled = false;
        hp = hpMax = hp_;
    }

    virtual void receiveShotDamage(ShotData& shot, double damage, Gem* pSourceGem) = 0;
    virtual void receiveShrineDamage(double damage) = 0;
    virtual double calculateIncomingDamage(double damage) { return damage; }
    virtual float getShotVariance() { return 0.0f; }
    virtual bool canBeTargeted() { return true; }
    virtual void setKillingShot() { isKillingShotOnTheWay = true; }

    float x;
    float y;

    int distanceToOrb;

    int incomingShots;
    double incomingDamage;
    bool isKillingShotOnTheWay;
    bool isKilled;

    double hp;
    double hpMax;
};
