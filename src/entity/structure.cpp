#include "entity/structure.h"
#include "entity/gem.h"

#include <algorithm>

Structure::Structure(int ix_, int iy_, int width_, int height_) : Targetable(100.0)
{
    ix = ix_;
    iy = iy_;
    width = width_;
    height = height_;

    x = 0.5f * width_ + ix_;
    y = 0.5f * width_ + iy_;

    armor = 100.0;

    isIndestructible = false;
}

void Structure::receiveShotDamage(ShotData& shot, double damage, double crit, Gem* pSourceGem)
{
    if (!isIndestructible)
    {
        if (isKilled)
            return;

        damage = std::max<double>(1, damage * (1.0 + crit) - armor);
        hp -= damage;

        if (pSourceGem != NULL)
            ++pSourceGem->hits;

        if (hp < 1.0)
        {
            isKilled = true;
        }
        else
        {
            if (shot.component[COMPONENT_ARMOR] > 0.0)
                armor = std::max(0.0, armor - shot.component[COMPONENT_ARMOR]);
        }
    }
}

double Structure::calculateIncomingDamage(double damage, double crit)
{
    return std::max<double>(1, damage * (1.0 + crit) - armor);
}
