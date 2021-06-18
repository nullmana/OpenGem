#include "entity/tower_shot.h"
#include "entity/gem.h"
#include "entity/tower.h"

#include "constants/game_header.h"

#include <cmath>

TowerShot::TowerShot(const Tower& tower, Targetable* pTarget_)
    : pTarget(pTarget_)
{
    pSourceGem = tower.pGem;
    shot = pSourceGem->shotFinal;
    damage = shot.rollDamage();
    crit = shot.rollCritMultiplier();
    isKillingShot = false;

    if (g_game.game == GC_LABYRINTH)
    {
        if (shot.component[COMPONENT_BLOODBOUND] > 0.0)
            damage += shot.component[COMPONENT_BLOODBOUND] * pSourceGem->kills;
    }

    const float scatter = pTarget->getShotVariance();
    if (scatter > 0.0f)
    {
        scatterX = 2.0f * (rand() / float(RAND_MAX)) * scatter - scatter;
        scatterY = 2.0f * (rand() / float(RAND_MAX)) * scatter - scatter;
    }
    else
    {
        scatterX = scatterY = 0.0f;
    }

    lastTargetX = pTarget_->x + scatterX;
    lastTargetY = pTarget_->y + scatterY;
    x = tower.x;
    y = tower.y;

    scaleBonus = std::min(0.8f, 0.08f * pSourceGem->grade);
    scale = 1.0f;
    angle = 0.0f;

    RGB = tower.pGem->RGB;

    if (g_game.game == GC_LABYRINTH)
    {
        z = 50.0f;

        velX = (50.0f / 33.0f) * (rand() / float(RAND_MAX)) - (25.0f / 33.0f);
        velY = (50.0f / 33.0f) * (rand() / float(RAND_MAX)) - (25.0f / 33.0f);
        velZ = 0.0f;
    }
    else if (g_game.game == GC_CHASINGSHADOWS)
    {
        z = 60.0f;

        velX = (14.0f / 17.0f) * (rand() / float(RAND_MAX)) - (7.0f / 17.0f);
        velY = (14.0f / 17.0f) * (rand() / float(RAND_MAX)) - (10.0f / 17.0f);
        velZ = 0.35f * (rand() / float(RAND_MAX)) + 0.1f;
    }
    else
    {
        throw "Game Code Unavailable!";
    }
}

bool TowerShot::tick(int frames)
{
    float lastX = x;
    float lastY = y;
    // Update every frame in case pTarget is lost
    if (pTarget != NULL)
    {
        lastTargetX = pTarget->x + scatterX;
        lastTargetY = pTarget->y + scatterY;
    }

    if (g_game.game == GC_LABYRINTH)
    {
        for (int f = 0; f < frames; ++f)
        {
            z = fmax(z + velZ, 0);
            velX *= 0.6f;
            velY *= 0.6f;
            velZ = fmax(velZ - 0.5f, -4.0f);
            x += velX;
            y += velY;

            x += (1.0f - z / 50.0f) * (lastTargetX - x);
            y += (1.0f - z / 50.0f) * (lastTargetY - y);

            if ((z < 20.0f) && (fabs(lastTargetX - x) + fabs(lastTargetY - y) < (4.0f / 33.0f)))
            {
                // Set position to target so chain hit based on shot position is most accurate
                x = lastTargetX;
                y = lastTargetY;
                return true;
            }
        }

        if (frames > 0)
        {
            scale = z * 0.03f + 0.8f;
            angle = atan2(lastY - y, lastX - x);
        }
    }
    else if (g_game.game == GC_CHASINGSHADOWS)
    {
        for (int f = 0; f < frames; ++f)
        {
            z = fmax(z + velZ, 30);
            velX *= 0.9f;
            velY *= 0.9f;
            velZ = fmax(velZ - 0.07f, -1.0f);
            x += velX;
            y += velY;

            float proximity =
                0.5f + fmax(0, fmax(((40.0f / 17.0f) - fabs(lastTargetX - x)) / (80.0f / 17.0f),
                                   ((40.0f / 17.0f) - fabs(lastTargetY - y)) / (80.0f / 17.0f)));
            x += proximity * (1.0f - z / 60.0f) * (lastTargetX - x);
            y += proximity * (1.0f - z / 60.0f) * (lastTargetY - y);

            if (fabs(lastTargetX - x) + fabs(lastTargetY - y) < (9.0f / 17.0f))
            {
                x = lastTargetX;
                y = lastTargetY;
                return true;
            }
        }

        if (frames > 0)
        {
            scale = z * 0.035f + scaleBonus + 0.7f;
            angle = atan2(lastY - y, lastX - x);
        }
    }
    else
    {
        throw "Game Code Unavailable!";
    }
    return false;
}
