#include "entity/mana_shard.h"

#include "ingame/ingame_mana_pool.h"

#include "entity/gem.h"

#include <cmath>
#ifdef DEBUG
#include <cstdio>
#endif

ManaShard::ManaShard(IngameManaPool& manaPool_, int ix_, int iy_, int width_, double mana_, double shell_, bool isCorrupted_)
    : Structure(ix_, iy_, width_, width_),
      manaPool(manaPool_)
{
    isCorrupted = isCorrupted_;
    incomingDamageOnShell = 0.0;

    if (isCorrupted_)
    {
        hpMax = hp = 1.0;
        armor = 0.0;
    }
    else
    {
        hpMax = hp = mana_;
        armor = shell_;
    }
}

uint32_t ManaShard::receiveShotDamage(ShotData& shot, uint32_t numShots, double damage, double crit, Gem* pSourceGem, bool isKillingShot)
{
    if (isIndestructible || isKilled)
        return 0;

    uint32_t shotsTaken = 0;
    double mana = 0.0;

    if (isCorrupted)
    {
        while (shotsTaken < numShots)
        {
            double modifiedDamage = std::max<double>(1.0, pow(damage * crit, 0.6) * pow(0.982, hpMax * 0.001));
            mana += modifiedDamage;
            hpMax += modifiedDamage;
            ++shotsTaken;

            if (pSourceGem != NULL)
            {
                pSourceGem->shotRaw.damageMin = std::max<double>(1.0, pSourceGem->shotRaw.damageMin * 0.98);
                pSourceGem->shotRaw.damageMax = std::max<double>(2.0, pSourceGem->shotRaw.damageMax * 0.99);
                pSourceGem->recalculateShotData();
            }
        }
    }
    else
    {
        while ((shotsTaken < numShots) && !isKilled)
        {
            double modifiedDamage = std::max<double>(1.0, pow(damage * crit, 0.6));
            if (armor > 0.0)
            {
                armor = std::max<double>(0.0, armor - modifiedDamage);
            }
            else
            {
                if (modifiedDamage >= hp)
                {
                    modifiedDamage = hp;
                    hp = 0.0;
                    isKilled = true;
                }
                else
                {
                    hp -= modifiedDamage;
                }

                mana += modifiedDamage;
            }

            ++shotsTaken;
        }
    }

    if (pSourceGem != NULL)
        pSourceGem->hits += shotsTaken;

    // Only corrupted shards apply multiplier
    manaPool.addMana(mana, isCorrupted);

    return shotsTaken;
}

double ManaShard::calculateIncomingDamage(double damage, double crit)
{
    if (isCorrupted)
        return 0.0;
    else
        return std::max<double>(1.0, pow(damage * crit, 0.6));
}

void ManaShard::addIncomingDamage(double damage)
{
    ++incomingShots;
    if (incomingDamageOnShell >= armor)
        incomingDamage += damage;
    else
        incomingDamageOnShell += damage;
}

void ManaShard::removeIncomingDamage(double damage)
{
    if (--incomingShots == 0)
    {
        incomingDamage = 0.0;
        incomingDamageOnShell = 0.0;
    }
    else
    {
        if (armor > 0.0)
            incomingDamageOnShell = std::max<double>(0.0, incomingDamageOnShell - damage);
        else
            incomingDamage -= damage;
    }
}

#ifdef DEBUG
void ManaShard::debugPrint() const
{
    printf(isCorrupted ? "Corrupted Shard:\n" : "Mana Shard:\n");
    printf("\tHP: %lf/%lf | Armor: %lf\n", hp, hpMax, armor);
}
#endif
