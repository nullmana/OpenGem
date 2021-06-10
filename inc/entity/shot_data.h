#pragma once

struct ShotData
{
public:
    ShotData();

    double damageMin;
    double damageMax;
    float range;
    float fireRate;

    float rangeSq() const { return range * range; }

    ShotData operator+(const ShotData& other) const;
    ShotData& operator+=(const ShotData& other);
    ShotData multiply(float dm, float rm, float fm, float sm) const;
    ShotData round() const;

#ifdef DEBUG
    void debugPrint() const;
#endif
};
