#pragma once

#include "constants/beacon_type.h"

#include "entity/structure.h"

#include <list>

class IngameMap;

class Beacon : public Structure
{
private:
    int16_t nextShotTimer;

public:
    Beacon(int ix_, int iy_, BEACON_TYPE beaconType_);

    virtual float getShotVariance() { return 0.3f * width; }

    bool isPassive() const { return (type == BEACON_PROTECTOR) || (type == BEACON_STATIC); }
    bool isProtector() const { return (type == BEACON_PROTECTOR); }

    bool tick(IngameMap& map, int frames);

    virtual uint32_t receiveShotDamage(ShotData& shot, uint32_t numShots, double damage, double crit,
        Gem* pSourceGem, bool isKillingShot);
    virtual void receiveBombDamage(const ShotData& shot, double damage);

    const BEACON_TYPE beaconType;
    float range;

    std::list<Beacon*> protecting;

#ifdef DEBUG
    virtual void debugPrint() const;
#endif
};
