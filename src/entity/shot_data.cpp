#include "entity/shot_data.h"

#include <cmath>

#ifdef DEBUG
#include <cstdio>
#endif

ShotData::ShotData()
{
    damageMin = 0.0;
    damageMax = 0.0;
    range = 0.0f;
    fireRate = 0.0f;
}

ShotData ShotData::operator+(const ShotData& other) const
{
    ShotData out;

    out.damageMin = damageMin + other.damageMin;
    out.damageMax = damageMax + other.damageMax;
    out.range = range + other.range;
    out.fireRate = fireRate + other.fireRate;

    return out;
}

ShotData& ShotData::operator+=(const ShotData& other)
{
    damageMin += other.damageMin;
    damageMax += other.damageMax;
    range += other.range;
    fireRate += other.fireRate;

    return *this;
}

ShotData ShotData::multiply(float dm, float rm, float fm, float sm) const
{
    ShotData out;

    out.damageMin = dm * damageMin;
    out.damageMax = dm * damageMax;
    out.range = rm * range;
    out.fireRate = fm * fireRate;

    return out;
}

ShotData ShotData::round() const
{
    ShotData out;

    out.damageMin = std::round(damageMin);
    out.damageMax = std::round(damageMax);
    out.range = 0.1f * std::round(10.0f * range);
    out.fireRate = 0.01f * std::round(100.0f * fireRate);

    return out;
}

#ifdef DEBUG
void ShotData::debugPrint() const
{
    printf("D: (%f - %f) | R: %f | F: %f\n", damageMin, damageMax, range, fireRate);
}
#endif
