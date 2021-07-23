#pragma once

#include "entity/structure.h"

class IngameManaPool;

class ManaShard : public Structure
{
private:
    IngameManaPool& manaPool;
    double incomingDamageOnShell;

public:
    ManaShard(IngameManaPool& manaPool_, int ix_, int iy_, int width_, double mana_, double shell_, bool isCorrupted_);

    bool isCorrupted;

    virtual uint32_t receiveShotDamage(ShotData& shot, uint32_t numShots, double damage, double crit,
        Gem* pSourceGem, bool isKillingShot);
    virtual void receiveBombDamage(const ShotData& shot, double damage) {}

    virtual double calculateIncomingDamage(double damage, double crit);
    virtual void addIncomingDamage(double damage);
    virtual void removeIncomingDamage(double damage);

    virtual float getShotVariance() { return 0.3f * width; }

#ifdef DEBUG
    virtual void debugPrint() const;
#endif
};
