#pragma once

struct ShotData
{
public:
    double damageMin;
    double damageMax;
    float range;
    float fireRate;

    float rangeSq() const { return range * range; }
};
