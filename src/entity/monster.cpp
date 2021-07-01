#include "entity/monster.h"
#include "entity/gem.h"
#include "entity/monster_node.h"

#include "ingame/ingame_level_definition.h"
#include "ingame/ingame_map.h"

#include <cmath>
#ifdef DEBUG
#include <cstdio>
#endif

#define HEALTH_BAR_FADEOUT_TIME 30

Monster::Monster(const MonsterSpawnNode* pStart, const MonsterNode* pTarget, const MonsterPrototype& mp)
    : Targetable(mp.hp)
{
    pSourceNode = pStart;
    pTargetNode = pTarget;

    color = 0xFFFFFF * (rand() / float(RAND_MAX));
    speedMax = mp.speed;

    armor = mp.armor;
    mana = mp.mana;
    banishmentCostMultiplier = mp.banishmentCostMultiplier;
    sortBanishmentCost = getBanishmentCost();

    shield = 0;

    type = mp.type;
    shockImmunity = 0.0;
    poisonDamage = 0.0;

    slowTimer = 0;
    poisonTimer = 0;
    healthBarTimer = 0;
    killingShotTimer = 0;
    shockTimer = 0;

    motionCycle = rand() % 10;

    switch (type)
    {
        case TARGET_REAVER:
        case TARGET_ARMORED:
        case TARGET_RUNNER:
            scale = 0.4f * g_game.ingameBuildingSize;
            break;
        case TARGET_SWARMLING:
        case TARGET_SPAWNLING:
            scale = 0.2f * g_game.ingameBuildingSize;
            break;
        case TARGET_GIANT:
            scale = 0.72f * g_game.ingameBuildingSize;
            break;
    }

    spawn();
}

void Monster::spawn()
{
    isKillingShotOnTheWay = false;
    incomingShots = 0;
    incomingShotsOnShield = 0;
    incomingDamage = 0.0;

    slowTimer = 0;
    manaBindTimer = 0;
    invulnerabilityTimer = 0;
    shockTimer = 0;
    speed = speedMax;

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

uint32_t Monster::receiveShotDamage(ShotData& shot, uint32_t numShots, double damage, double crit, Gem* pSourceGem, bool isKillingShot)
{
    uint32_t shotsTaken = 0;

    if (isKilled || (invulnerabilityTimer > 0))
        return 0;

    if (shield > 0)
    {
        if (numShots <= shield)
        {
            shield -= numShots;
            return numShots;
        }
        else
        {
            shotsTaken = shield;
            if (pSourceGem != NULL)
                pSourceGem->hits += shotsTaken;
            shield = 0;
            return shotsTaken + receiveShotDamage(shot, numShots - shotsTaken, damage, crit, pSourceGem, isKillingShot);
        }
    }

    double modifiedDamage = std::max<double>(1.0, damage * (1.0 + crit) - armor);
    if (isKillingShotOnTheWay && isKillingShot)
    {
        damage += 2.0 * hpMax;
        hp -= damage;
        shotsTaken = 1;
    }
    else if (shot.component[COMPONENT_ARMOR] <= 0.0)
    {
        if (hp < numShots * modifiedDamage)
        {
            shotsTaken = ceil(hp / modifiedDamage);
            hp -= shotsTaken * modifiedDamage;
        }
        else
        {
            hp -= numShots * modifiedDamage;
            shotsTaken = numShots;
        }
    }
    else
    {
        while (shotsTaken < numShots)
        {
            hp -= modifiedDamage;
            ++shotsTaken;

            if ((shot.component[COMPONENT_ARMOR] > 0.0) && (armor > 0.0))
            {
                armor = std::max(0.0, armor - shot.component[COMPONENT_ARMOR]);
                modifiedDamage = std::max<double>(1.0, damage * (1.0 + crit) - armor);
            }

            if (hp < 1.0)
                break;
        }
    }

    if (pSourceGem != NULL)
        pSourceGem->hits += shotsTaken;

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
        if ((g_game.game == GC_LABYRINTH) && (shockTimer <= 0) &&
            (shot.component[COMPONENT_SHOCK] > 0.0))
        {
            double totalChance = 1.0 - pow(1.0 - (shot.component[COMPONENT_SHOCK] - shockImmunity), shotsTaken);
            if (totalChance > (rand() / double(RAND_MAX)))
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
                for (uint32_t i = 0; i < shotsTaken; ++i)
                {
                    a = (a + b * b / (a + b));
                }
                poisonDamage = a / (9.0 * 30.0);
                poisonTimer = 9 * 30;
            }
        }
    }

    return shotsTaken;
}

void Monster::receiveShrineDamage(double damage)
{
    if (isKilled || (invulnerabilityTimer > 0))
        return;

    hp -= damage;

    if (hp < hpMax)
        healthBarTimer = HEALTH_BAR_FADEOUT_TIME;
}

void Monster::receiveBombDamage(const ShotData& shot, double damage)
{
    ShotData sd = shot;
    if (g_game.game == GC_LABYRINTH)
        sd.component[COMPONENT_SHOCK] = 0.1;

    receiveShotDamage(sd, 1, damage, 0.0, NULL, false);
}

double Monster::calculateIncomingDamage(double damage, double crit)
{
    if (invulnerabilityTimer > 0)
        return 0.0;
    if (shield > incomingShotsOnShield)
        return 0.0;
    return std::max<double>(1, damage * (1.0 + crit) - armor);
}

void Monster::addIncomingDamage(double damage)
{
    ++incomingShots;
    if (invulnerabilityTimer <= 0)
    {
        if (shield > incomingShotsOnShield)
            ++incomingShotsOnShield;
        else
            incomingDamage += damage;
    }
}

void Monster::removeIncomingDamage(double damage)
{
    if (--incomingShots == 0)
    {
        incomingShotsOnShield = 0;
        incomingDamage = 0;
    }
    else
    {
        if (invulnerabilityTimer <= 0)
        {
            if (shield > 0)
                --incomingShotsOnShield;
            else
                incomingDamage = std::max(0.0, incomingDamage - damage);
        }
    }
}

void Monster::receiveBeaconHeal(double heal)
{
    hp = std::min<double>(hpMax, hp + heal);
    if (hp == hpMax)
        healthBarTimer = 0;
}

void Monster::receiveBeaconHaste()
{
    float speedBoost;
    float speedCap;
    if (g_game.game == GC_LABYRINTH)
    {
        speedBoost = 1.2f;
        if (isHoppingGCL())
            speedCap = 5.0f;
        else if (isCrawlingGCL())
            speedCap = 4.0f;
        else
            speedCap = 6.0f;
    }
    else
    {
        speedBoost = 1.1f;
        speedCap = 5.0f;
    }

    speed = std::min(speedCap, speed * speedBoost);
    speedMax = std::min(speedCap, speedMax * speedBoost);

    adjustSpeedAngle();
}

void Monster::receiveBeaconCleanse()
{
    slowTimer = 0;
    poisonTimer = 0;
    shockTimer = 0;
}

void Monster::receiveBeaconInvulnerability()
{
    invulnerabilityTimer = rand() % 200 + 200;
    incomingDamage = 0.0;
}

void Monster::receiveBeaconShield()
{
    ++shield;
}

void Monster::receiveBeaconManaBind()
{
    manaBindTimer = rand() % 200 + 200;
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

void Monster::multiplyBanishmentCost(double mult)
{
    banishmentCostMultiplier *= mult;
    sortBanishmentCost = getBanishmentCost();
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
    float speedFloatFactor = (g_game.game == GC_LABYRINTH) ? (1.0f / 33.0f) : (1.0f / 17.0f);

    speedCos = speed * cos(motionAngle) * speedFloatFactor;
    speedSin = speed * sin(motionAngle) * speedFloatFactor;
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
        if (manaBindTimer > 0)
            manaBindTimer -= frames;
        if (invulnerabilityTimer > 0)
            invulnerabilityTimer -= frames;
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
                float speedFactor = 1.0f;

                if (g_game.game == GC_LABYRINTH)
                {
                    if (isHoppingGCL())
                    {
                        if (motionCycle == 0)
                            speedFactor = 0.0f;
                        else
                            speedFactor = std::max(0.0f, (12 - motionCycle) / 17.0f) * 1.65f;

                        if (++motionCycle > 18)
                            motionCycle = rand() % 3 - 2;
                    }
                    else if (isCrawlingGCL())
                    {
                        speedFactor = pow(sin(motionCycle * (M_PI / 50.0f)), 2.0f);

                        if (++motionCycle > 50)
                            motionCycle = 0;
                    }
                }

                x += speedCos * speedFactor;
                y += speedSin * speedFactor;
                distanceToOrb -= 100 * speed * speedFactor;

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

#ifdef DEBUG
void Monster::debugPrint() const
{
    printf("Monster ");
    switch (type)
    {
        case TARGET_REAVER:
            printf("(Reaver):\n");
            break;
        case TARGET_SWARMLING:
            printf("(Swarmling):\n");
            break;
        case TARGET_SPAWNLING:
            printf("(Spawnling):\n");
            break;
        case TARGET_GIANT:
            printf("(Giant):\n");
            break;
        case TARGET_ARMORED:
            printf("(Armored):\n");
            break;
        case TARGET_RUNNER:
            printf("(Runner):\n");
            break;
        default:
            printf("(Invalid Type %x):\n", type);
            break;
    }
    printf("\tHP: %lf/%lf | Armor: %lf\n", hp, hpMax, armor);
    if (shield > 0)
        printf("\tShield: %i\n", shield);
    if (slowTimer > 0)
        printf("\tSlowed: %i\n", slowTimer);
    if (poisonTimer > 0)
        printf("\tPoisoned: %i@%f\n", poisonTimer, poisonDamage);
    if (manaBindTimer > 0)
        printf("\tMana Bind: %i\n", manaBindTimer);
    if (invulnerabilityTimer > 0)
        printf("\tInvulnerable: %i\n", invulnerabilityTimer);
    if (shockTimer > 0)
        printf("\tShocked: %i\n", shockTimer);
}
#endif

#undef HEALTH_BAR_FADEOUT_TIME
