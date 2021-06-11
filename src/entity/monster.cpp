#include "entity/monster.h"
#include "entity/gem.h"
#include "entity/monster_node.h"

#include "ingame/ingame_level_definition.h"
#include "ingame/ingame_map.h"

#include <cmath>

#include <cstdio>

#define MONSTER_SPEED_FLOAT_FACTOR (1.0f / 17.0f)
#define HEALTH_BAR_FADEOUT_TIME 30

Monster::Monster(
    const MonsterSpawnNode* pStart, const MonsterNode* pTarget, const MonsterPrototype& mp)
    : Targetable(std::min(mp.hp, 1E300))
{
    pSourceNode = pStart;
    pTargetNode = pTarget;

    color = 0xFFFFFF * (rand() / float(RAND_MAX));
    speedMax = 0.4f + (rand() / float(RAND_MAX));

    armor = mp.armor;
    mana = mp.mana;
    banishmentCostMultiplier = mp.banishmentCostMultiplier;
    shockImmunity = 0.0;
    poisonDamage = 0.0;

    slowTimer = 0;
    poisonTimer = 0;
    healthBarTimer = 0;
    killingShotTimer = 0;
    shockTimer = 0;

    spawn();
}

void Monster::spawn()
{
    isKillingShotOnTheWay = false;
    incomingShots = 0;
    incomingDamage = 0.0;

    slowTimer = 0;
    speed = speedMax;
    shockTimer = 0;

    x = pSourceNode->spawnX;
    y = pSourceNode->spawnY;

    int ix = (int)x;
    int iy = (int)y;

    nextX = ix;
    nextY = iy;

    if (nextX < 0)
        nextX = 0;
    else if (nextX > g_game.ingameMapWidth - 1)
        nextX = g_game.ingameMapWidth - 1;
    if (nextY < 0)
        nextY = 0;
    else if (nextY > g_game.ingameMapHeight - 1)
        nextY = g_game.ingameMapHeight - 1;

    if ((ix != nextX) || (iy != nextY))
    {
        // Started off-screen, need to reach map first
        distanceToOrb = 1000000 * pTargetNode->tileDistance.at(nextY, nextX) + 10000;
        setNextTarget(nextX, nextY);
    }
    else
    {
        // Started on-screen, pick next node immediately
        pickNextTarget();
    }
}

void Monster::receiveShotDamage(ShotData& shot, double damage, double crit, Gem* pSourceGem)
{
    if (isKilled)
        return;

    damage = std::max<double>(1.0, damage * (1.0 + crit) - armor);
    if (isKillingShotOnTheWay)
        damage += 2.0 * hpMax;

    hp -= damage;

    if (pSourceGem != NULL)
        ++pSourceGem->hits;

    if (hp < hpMax)
        healthBarTimer = HEALTH_BAR_FADEOUT_TIME;

    if (hp < 1.0)
    {
        isKilled = true;

        if (pSourceGem != NULL)
        {
            ++pSourceGem->kills;
            ++pSourceGem->killsNonCombined;
        }
    }
    else
    {
        if (shot.component[COMPONENT_ARMOR] > 0.0)
            armor = std::max(0.0, armor - shot.component[COMPONENT_ARMOR]);

        if ((g_game.game == GC_LABYRINTH) && (shockTimer <= 0) &&
            (shot.component[COMPONENT_SHOCK] > 0.0))
        {
            if (shot.component[COMPONENT_SHOCK] - shockImmunity > (rand() / double(RAND_MAX)))
            {
                shockTimer = 90;
                shockImmunity += 0.05 + 0.07 * (rand() / double(RAND_MAX));
                shockX = x;
                shockY = y;
            }
        }

        if (shot.component[COMPONENT_SLOW_POWER] > 0.0)
        {
            if ((slowTimer <= 0) || (shot.component[COMPONENT_SLOW_POWER] < (speed / speedMax)))
            {
                speed = speedMax * shot.component[COMPONENT_SLOW_POWER];
                adjustSpeedAngle();
            }
        }

        if (shot.component[COMPONENT_POISON] > 0.0)
        {
            if (g_game.game == GC_LABYRINTH)
            {
                if (poisonTimer * poisonDamage < shot.component[COMPONENT_POISON] * 150.0)
                {
                    poisonTimer = 150;
                    poisonDamage = shot.component[COMPONENT_POISON] / 150.0;
                }
            }
            else
            {
                double a = poisonDamage * poisonTimer;
                double b = shot.component[COMPONENT_POISON];
                poisonDamage = (a + b * b / (a + b)) / (9.0 * 30.0);
                poisonTimer = 9 * 30;
            }
        }
    }
}

void Monster::receiveShrineDamage(double damage)
{
    if (isKilled)
        return;

    hp -= damage;

    if (hp < hpMax)
        healthBarTimer = HEALTH_BAR_FADEOUT_TIME;
}

double Monster::calculateIncomingDamage(double damage, double crit)
{
    return std::max<double>(1, damage * (1.0 + crit) - armor);
}

void Monster::pickNextTarget()
{
    int ix = (int)x;
    int iy = (int)y;

    if (pTargetNode->nodeType == TILE_ORB)
        distanceToOrb = 1000000 * pTargetNode->tileDistance.at(iy, ix) + 10000;
    else if (pSourceNode->nodeType == TILE_ORB)
        distanceToOrb = 1000000 * pSourceNode->tileDistance.at(iy, ix) + 10000;
    else
        distanceToOrb = INT_MAX;

    int nextDirection = MonsterNode::pickDirection(pTargetNode->tileDirection.at(iy, ix));
    if (nextDirection != -1)
    {
        ix += PathWeight::dx[nextDirection];
        iy += PathWeight::dy[nextDirection];
    }
    setNextTarget(ix, iy);
}

void Monster::setNextTarget(int nx, int ny)
{
    nextX = nx;
    nextY = ny;

    nearNextX = nextX + 0.2f + 0.6f * (rand() / float(RAND_MAX));
    nearNextY = nextY + 0.2f + 0.6f * (rand() / float(RAND_MAX));

    adjustMotionAngle();
}

void Monster::adjustMotionAngle()
{
    motionAngle = atan2(nearNextY - y, nearNextX - x);

    adjustSpeedAngle();
}

void Monster::adjustSpeedAngle()
{
    speedCos = speed * cos(motionAngle) * MONSTER_SPEED_FLOAT_FACTOR;
    speedSin = speed * sin(motionAngle) * MONSTER_SPEED_FLOAT_FACTOR;
}

bool Monster::tick(IngameMap& map, int frames)
{
    if (isKilled)
        return true;

    if (frames > 0)
    {
        if (healthBarTimer > 0)
            --healthBarTimer;
        if ((killingShotTimer > 0) && ((killingShotTimer -= frames) <= 0))
            isKillingShotOnTheWay = false;
        if (shockTimer > 0)
            shockTimer -= frames;
        if ((slowTimer > 0) && (slowTimer -= frames) <= 0)
        {
            speed = speedMax;
            adjustSpeedAngle();
        }
        if ((frames > 0) && (poisonTimer > 0))
        {
            double damage = frames * poisonDamage;
            hp -= damage;
            poisonTimer -= frames;
            healthBarTimer = HEALTH_BAR_FADEOUT_TIME;
            if (hp < 1.0)
            {
                isKilled = true;
                return true;
            }
        }

        if (shockTimer > 0)
        {
            shockX = x + (rand() / float(RAND_MAX)) * 0.1f - 0.05f;
            shockY = y + (rand() / float(RAND_MAX)) * 0.1f - 0.05f;
        }
        else
        {
            for (int f = 0; f < frames; ++f)
            {
                x += speedCos;
                y += speedSin;
                distanceToOrb -= 100 * speed;

                int ix = (int)x;
                int iy = (int)y;

                if ((ix == nextX) && (iy == nextY))
                {
                    if (pTargetNode->tileDirection.at(nextY, nextX).sum == 0)
                    {
                        // On zero weight tile, assume reached destination
                        map.monsterReachesTarget(*this);
                        if (isKilled)
                            return true;
                    }
                    else
                    {
                        pickNextTarget();
                    }
                }
            }
        }
    }

    return isKilled;
}
