#include "entity/structure.h"
#include "entity/gem.h"

#include <algorithm>

Structure::Structure(int ix_, int iy_, int width_, int height_) : Targetable(100.0)
{
    ix = ix_;
    iy = iy_;
    width = width_;
    height = height_;

    armor = 100.0;

    isIndestructible = false;
}

void Structure::receiveShotDamage(ShotData& shot, double damage, Gem* pSourceGem)
{
    if (!isIndestructible)
    {
        if (isKilled)
            return;

        damage = std::max<double>(1, damage - armor);
        hp -= damage;

        if (pSourceGem != NULL)
            ++pSourceGem->hits;

        if (hp < 1.0)
            isKilled = true;
    }
}

double Structure::calculateIncomingDamage(double damage)
{
    return std::max<double>(1, damage - armor);
}
