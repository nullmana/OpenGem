#pragma once

#include "constants/gem_component_type.h"

struct ShotData
{
public:
    ShotData();

    double damageMin;
    double damageMax;
    float range;
    float fireRate;

    double component[GEM_COMPONENT_INDEX_COUNT];

    float rangeSq() const { return range * range; }
    double rollDamage() const;

    // Manipulate ShotData for amplification
    // Does not affect critical chance or slow duration
    ShotData addExisting(const ShotData& other) const;
    ShotData operator+(const ShotData& other) const;
    ShotData& operator+=(const ShotData& other);
    ShotData multiply(float dm, float rm, float fm, float sm) const;
    ShotData round() const;

#ifdef DEBUG
    void debugPrint() const;
#endif
};
