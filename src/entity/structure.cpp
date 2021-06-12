#include "entity/structure.h"
#include "entity/gem.h"

#include "constants/target_type.h"

#include <algorithm>

Structure::Structure(int ix_, int iy_, int width_, int height_)
    : Targetable(100.0)
{
    ix = ix_;
    iy = iy_;
    width = width_;
    height = height_;

    type = TARGET_STRUCTURE;

    x = 0.5f * width_ + ix_;
    y = 0.5f * width_ + iy_;

    armor = 100.0;

    isIndestructible = false;
}

uint32_t Structure::receiveShotDamage(ShotData& shot, uint32_t numShots, double damage, double crit, Gem* pSourceGem, bool isKillingShot)
{
    if (!isIndestructible)
    {
        uint32_t shotsTaken = 0;

        if (isKilled)
            return 0;

        double modifiedDamage = std::max<double>(1, damage * (1.0 + crit) - armor);
        while (shotsTaken < numShots)
        {
            hp -= damage;
            ++shotsTaken;

            if ((shot.component[COMPONENT_ARMOR] > 0.0) && (armor > 0.0))
            {
                armor = std::max(0.0, armor - shot.component[COMPONENT_ARMOR]);
                modifiedDamage = std::max<double>(1, damage * (1.0 + crit) - armor);
            }

            if (hp < 1.0)
            {
                isKilled = true;
                break;
            }
        }

        if (pSourceGem != NULL)
            pSourceGem->hits += shotsTaken;

        return shotsTaken;
    }

    return 0;
}

double Structure::calculateIncomingDamage(double damage, double crit)
{
    return std::max<double>(1, damage * (1.0 + crit) - armor);
}
