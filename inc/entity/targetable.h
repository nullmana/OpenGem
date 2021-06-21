#pragma once

#include <climits>
#include <cstdint>

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
        isSelectedTarget = false;
        hp = hpMax = hp_;
        armor = 0.0;
        sortBanishmentCost = 0.0;
    }

    virtual uint32_t receiveShotDamage(ShotData& shot, uint32_t numShots, double damage, double crit,
        Gem* pSourceGem, bool isKillingShot) = 0;
    virtual void receiveShrineDamage(double damage) = 0;
    virtual void receiveBombDamage(const ShotData& shot, double damage) = 0;
    virtual double calculateIncomingDamage(double damage, double crit) { return damage; }
    virtual float getShotVariance() { return 0.0f; }
    virtual bool canBeTargeted() { return true; }
    virtual bool canLeech() { return true; }
    virtual void setKillingShot() { isKillingShotOnTheWay = true; }

    float x;
    float y;

    int distanceToOrb;
    uint32_t type;

    int incomingShots;
    double incomingDamage;
    bool isKillingShotOnTheWay;
    bool isKilled;
    bool isSelectedTarget;

    double hp;
    double hpMax;
    double armor;
    double sortBanishmentCost;

#ifdef DEBUG
    virtual void debugPrint() const
    {
    }
#endif
};
