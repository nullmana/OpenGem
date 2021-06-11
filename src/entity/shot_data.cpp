#include "entity/shot_data.h"

#include "constants/game_header.h"

#include <cmath>
#include <cstring>

#ifdef DEBUG
#include <cstdio>
#endif

ShotData::ShotData()
{
    damageMin = 0.0;
    damageMax = 0.0;
    range = 0.0f;
    fireRate = 0.0f;

    memset(component, 0, sizeof(component));
}

double ShotData::rollDamage() const
{
    return std::round((damageMax - damageMin) * (rand() / double(RAND_MAX)) + damageMin);
}

double ShotData::rollCritMultiplier() const
{
    if (component[COMPONENT_CRITICAL_POWER] <= 0.0)
        return 0.0;

    if (g_game.game == GC_LABYRINTH)
    {
        double multiplier = floor(component[COMPONENT_CRITICAL_POWER]);
        if ((rand() / double(RAND_MAX)) <= fmod(component[COMPONENT_CRITICAL_POWER], 1.0))
            multiplier += 1.0;
        return multiplier;
    }
    else
    {
        if ((rand() / double(RAND_MAX)) <= component[COMPONENT_CRITICAL_CHANCE])
            return component[COMPONENT_CRITICAL_POWER];
        else
            return 0.0;
    }
}

ShotData ShotData::addExisting(const ShotData& other) const
{
    ShotData out = other;

    out.damageMin += damageMin;
    out.damageMax += damageMax;
    out.range += range;
    out.fireRate += fireRate;

    if ((component[COMPONENT_CHAIN] > 0.0) && (other.component[COMPONENT_CHAIN] > 0.0))
        out.component[COMPONENT_CHAIN] += component[COMPONENT_CHAIN];
    if ((component[COMPONENT_POISON] > 0.0) && (other.component[COMPONENT_POISON] > 0.0))
        out.component[COMPONENT_POISON] += component[COMPONENT_POISON];
    if ((component[COMPONENT_ARMOR] > 0.0) && (other.component[COMPONENT_ARMOR] > 0.0))
        out.component[COMPONENT_ARMOR] += component[COMPONENT_ARMOR];
    if ((component[COMPONENT_SHOCK] > 0.0) && (other.component[COMPONENT_SHOCK] > 0.0))
        out.component[COMPONENT_SHOCK] += component[COMPONENT_SHOCK];
    if ((component[COMPONENT_BLOODBOUND] > 0.0) && (other.component[COMPONENT_BLOODBOUND] > 0.0))
        out.component[COMPONENT_BLOODBOUND] += component[COMPONENT_BLOODBOUND];
    if ((component[COMPONENT_POOLBOUND] > 0.0) && (other.component[COMPONENT_POOLBOUND] > 0.0))
        out.component[COMPONENT_POOLBOUND] += component[COMPONENT_POOLBOUND];
    if ((component[COMPONENT_LEECH] > 0.0) && (other.component[COMPONENT_LEECH] > 0.0))
        out.component[COMPONENT_LEECH] += component[COMPONENT_LEECH];
    if ((component[COMPONENT_SLOW_POWER] > 0.0) && (other.component[COMPONENT_SLOW_POWER] > 0.0))
        out.component[COMPONENT_SLOW_POWER] += component[COMPONENT_SLOW_POWER];
    if ((component[COMPONENT_CRITICAL_POWER] > 0.0) &&
        (other.component[COMPONENT_CRITICAL_POWER] > 0.0))
        out.component[COMPONENT_CRITICAL_POWER] += component[COMPONENT_CRITICAL_POWER];

    return out;
}

ShotData ShotData::operator+(const ShotData& other) const
{
    ShotData out;

    out.damageMin = damageMin + other.damageMin;
    out.damageMax = damageMax + other.damageMax;
    out.range = range + other.range;
    out.fireRate = fireRate + other.fireRate;

    out.component[COMPONENT_CHAIN] = component[COMPONENT_CHAIN] + other.component[COMPONENT_CHAIN];
    out.component[COMPONENT_POISON] =
        component[COMPONENT_POISON] + other.component[COMPONENT_POISON];
    out.component[COMPONENT_ARMOR] = component[COMPONENT_ARMOR] + other.component[COMPONENT_ARMOR];
    out.component[COMPONENT_SHOCK] = component[COMPONENT_SHOCK] + other.component[COMPONENT_SHOCK];
    out.component[COMPONENT_BLOODBOUND] =
        component[COMPONENT_BLOODBOUND] + other.component[COMPONENT_BLOODBOUND];
    out.component[COMPONENT_POOLBOUND] =
        component[COMPONENT_POOLBOUND] + other.component[COMPONENT_POOLBOUND];
    out.component[COMPONENT_LEECH] = component[COMPONENT_LEECH] + other.component[COMPONENT_LEECH];
    out.component[COMPONENT_SLOW_POWER] =
        component[COMPONENT_SLOW_POWER] + other.component[COMPONENT_SLOW_POWER];
    out.component[COMPONENT_SLOW_DURATION] = component[COMPONENT_SLOW_DURATION];
    out.component[COMPONENT_CRITICAL_POWER] =
        component[COMPONENT_CRITICAL_POWER] + other.component[COMPONENT_CRITICAL_POWER];

    return out;
}

ShotData& ShotData::operator+=(const ShotData& other)
{
    damageMin += other.damageMin;
    damageMax += other.damageMax;
    range += other.range;
    fireRate += other.fireRate;

    component[COMPONENT_CHAIN] += other.component[COMPONENT_CHAIN];
    component[COMPONENT_POISON] += other.component[COMPONENT_POISON];
    component[COMPONENT_ARMOR] += other.component[COMPONENT_ARMOR];
    component[COMPONENT_SHOCK] += other.component[COMPONENT_SHOCK];
    component[COMPONENT_BLOODBOUND] += other.component[COMPONENT_BLOODBOUND];
    component[COMPONENT_POOLBOUND] += other.component[COMPONENT_POOLBOUND];
    component[COMPONENT_LEECH] += other.component[COMPONENT_LEECH];
    component[COMPONENT_SLOW_POWER] += other.component[COMPONENT_SLOW_POWER];
    component[COMPONENT_CRITICAL_POWER] += other.component[COMPONENT_CRITICAL_POWER];

    return *this;
}

ShotData ShotData::multiply(float dm, float rm, float fm, float sm) const
{
    ShotData out;

    out.damageMin = dm * damageMin;
    out.damageMax = dm * damageMax;
    out.range = rm * range;
    out.fireRate = fm * fireRate;

    out.component[COMPONENT_CHAIN] = sm * component[COMPONENT_CHAIN];
    out.component[COMPONENT_POISON] = sm * component[COMPONENT_POISON];
    out.component[COMPONENT_ARMOR] = sm * component[COMPONENT_ARMOR];
    out.component[COMPONENT_SHOCK] = sm * component[COMPONENT_SHOCK];
    out.component[COMPONENT_BLOODBOUND] = sm * component[COMPONENT_BLOODBOUND];
    out.component[COMPONENT_POOLBOUND] = sm * component[COMPONENT_POOLBOUND];
    out.component[COMPONENT_LEECH] = sm * component[COMPONENT_LEECH];
    out.component[COMPONENT_SLOW_POWER] = sm * component[COMPONENT_SLOW_POWER];
    out.component[COMPONENT_SLOW_DURATION] = component[COMPONENT_SLOW_DURATION];
    out.component[COMPONENT_CRITICAL_POWER] = sm * component[COMPONENT_CRITICAL_POWER];
    out.component[COMPONENT_CRITICAL_CHANCE] = component[COMPONENT_CRITICAL_CHANCE];

    return out;
}

ShotData ShotData::round() const
{
    ShotData out;

    out.damageMin = std::round(damageMin);
    out.damageMax = std::round(damageMax);
    out.range = 0.1f * std::round(10.0f * range);
    out.fireRate = 0.01f * std::round(100.0f * fireRate);

    out.component[COMPONENT_CHAIN] = 0.01 * std::round(100.0 * component[COMPONENT_CHAIN]);
    out.component[COMPONENT_POISON] = 0.01 * std::round(100.0 * component[COMPONENT_POISON]);
    out.component[COMPONENT_ARMOR] = 0.01 * std::round(100.0 * component[COMPONENT_ARMOR]);
    out.component[COMPONENT_SHOCK] = 0.01 * std::round(100.0 * component[COMPONENT_SHOCK]);
    out.component[COMPONENT_BLOODBOUND] =
        0.01 * std::round(100.0 * component[COMPONENT_BLOODBOUND]);
    out.component[COMPONENT_POOLBOUND] = 0.01 * std::round(100.0 * component[COMPONENT_POOLBOUND]);
    out.component[COMPONENT_LEECH] = 0.1 * std::round(10.0 * component[COMPONENT_LEECH]);
    out.component[COMPONENT_SLOW_POWER] =
        0.01 * std::round(100.0 * component[COMPONENT_SLOW_POWER]);
    out.component[COMPONENT_SLOW_DURATION] = component[COMPONENT_SLOW_DURATION];
    out.component[COMPONENT_CRITICAL_POWER] =
        0.01 * std::round(100.0 * component[COMPONENT_CRITICAL_POWER]);
    out.component[COMPONENT_CRITICAL_CHANCE] = component[COMPONENT_CRITICAL_CHANCE];

    return out;
}

#ifdef DEBUG
void ShotData::debugPrint() const
{
    static const char* componentName[GEM_COMPONENT_INDEX_COUNT] = {
        "Slow Power",
        "Chain",
        "Poison",
        "Armor",
        g_game.game == GC_LABYRINTH ? "Shock" : "Suppress",
        "Bloodbound",
        "Critical Power",
        "Leech",
        "Poolbound",
        "Slow Duration",
        "Critical Chance",
    };

    printf("\tD: (%f - %f) | R: %f | F: %f\n", damageMin, damageMax, range, fireRate);
    for (int i = 0; i < GEM_COMPONENT_INDEX_COUNT; ++i)
    {
        if (component[i] > 0.0)
        {
            printf("\t\t%s: %lf\n", componentName[i], component[i]);
        }
    }
}
#endif
