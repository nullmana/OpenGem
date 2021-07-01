#include "entity/beacon.h"

#include "constants/game_header.h"

#include "ingame/ingame_map.h"

#include <cmath>

#ifdef DEBUG
#include <cstdio>
#endif

Beacon::Beacon(int ix_, int iy_, BEACON_TYPE beaconType_)
    : Structure(ix_, iy_, g_game.ingameBuildingSize, g_game.ingameBuildingSize),
      beaconType(beaconType_)
{
    if (g_game.game == GC_LABYRINTH)
    {
        range = 100.0f / 33.0f;
        nextShotTimer = rand() % 100 + 200;
    }
    else
    {
        range = 50.0f / 17.0f + 60.0f / 17.0f * (rand() / float(RAND_MAX));
        nextShotTimer = rand() % 240 + 180;
    }
}

bool Beacon::tick(IngameMap& map, int frames)
{
    if (isKilled)
        return true;

    if (!isPassive())
    {
        nextShotTimer -= frames;
        if (nextShotTimer <= 0)
        {
            switch (beaconType)
            {
                case BEACON_HEAL:
                    activateHeal(map);
                    break;
                case BEACON_SPEED:
                    activateSpeed(map);
                    break;
                case BEACON_CLEANSE:
                    activateCleanse(map);
                    break;
                case BEACON_INVULNERABILITY: // case BEACON_SHIELD:
                    if (g_game.game == GC_LABYRINTH)
                        activateInvulnerability(map);
                    else
                        activateShield(map);
                    break;
                case BEACON_MANA_BIND: // case BEACON_DISCHARGE:
                    if (g_game.game == GC_LABYRINTH)
                        activateManaBind(map);
                    else
                        activateDischarge(map);
                    break;
                case BEACON_OMNI:
                    activateOmni(map);
                    break;
            }
            if (g_game.game == GC_LABYRINTH)
                nextShotTimer = rand() % 100 + 200;
            else
                nextShotTimer = rand() % 240 + 180;
        }
    }

    return false;
}

uint32_t Beacon::receiveShotDamage(ShotData& shot, uint32_t numShots, double damage,
    double crit, Gem* pSourceGem, bool isKillingShot)
{
    if (isIndestructible || isKilled)
        return 0;

    if ((type != BEACON_PROTECTOR) && !protecting.empty())
        damage *= 0.25;

    return Structure::receiveShotDamage(shot, numShots, damage, crit, pSourceGem, isKillingShot);
}

void Beacon::receiveBombDamage(const ShotData& shot, double damage)
{
    if (isIndestructible || isKilled)
        return;

    if ((type != BEACON_PROTECTOR) && !protecting.empty())
        damage *= 0.25;

    Structure::receiveBombDamage(shot, damage);
}

void Beacon::activateHeal(IngameMap& map)
{
    std::vector<Targetable*> targets = map.enemyController.getBeaconTargetsWithinRangeSq(y, x, range * range, false);

    for (Targetable* t : targets)
    {
        Monster* pMonster = reinterpret_cast<Monster*>(t);

        double heal;
        if (g_game.game == GC_LABYRINTH)
            heal = 0.3 * (1.0 - (110.0 / 66.0) * sqrt((t->x - x) * (t->x - x) + (t->y - y) * (t->y - y)));
        else
            heal = 0.05;

        pMonster->receiveBeaconHeal(t->hpMax * heal);
    }
}

void Beacon::activateSpeed(IngameMap& map)
{
    std::vector<Targetable*> targets = map.enemyController.getBeaconTargetsWithinRangeSq(y, x, range * range, true);
    for (Targetable* t : targets)
    {
        Monster* pMonster = reinterpret_cast<Monster*>(t);
        pMonster->receiveBeaconHaste();
    }
}

void Beacon::activateCleanse(IngameMap& map)
{
    std::vector<Targetable*> targets = map.enemyController.getBeaconTargetsWithinRangeSq(y, x, range * range, false);
    for (Targetable* t : targets)
    {
        Monster* pMonster = reinterpret_cast<Monster*>(t);
        pMonster->receiveBeaconCleanse();
    }
}

void Beacon::activateInvulnerability(IngameMap& map)
{
    std::vector<Targetable*> targets = map.enemyController.getBeaconTargetsWithinRangeSq(y, x, range * range, false);
    for (Targetable* t : targets)
    {
        Monster* pMonster = reinterpret_cast<Monster*>(t);
        pMonster->receiveBeaconInvulnerability();
    }
}

void Beacon::activateManaBind(IngameMap& map)
{
    std::vector<Targetable*> targets = map.enemyController.getBeaconTargetsWithinRangeSq(y, x, range * range, true);
    for (Targetable* t : targets)
    {
        Monster* pMonster = reinterpret_cast<Monster*>(t);
        pMonster->receiveBeaconManaBind();
    }
}

void Beacon::activateShield(IngameMap& map)
{
    std::vector<Targetable*> targets = map.enemyController.getBeaconTargetsWithinRangeSq(y, x, range * range, false);
    for (Targetable* t : targets)
    {
        Monster* pMonster = reinterpret_cast<Monster*>(t);
        pMonster->receiveBeaconShield();
    }
}

void Beacon::activateDischarge(IngameMap& map)
{
    map.buildingController.dischargeGems(x, y, range * range);
}

void Beacon::activateOmni(IngameMap& map)
{
    std::vector<Targetable*> targets = map.enemyController.getBeaconTargetsWithinRangeSq(y, x, range * range, false);
    for (Targetable* t : targets)
    {
        Monster* pMonster = reinterpret_cast<Monster*>(t);
        pMonster->receiveBeaconHeal(t->hpMax * 0.05);
        pMonster->receiveBeaconHaste();
        pMonster->receiveBeaconCleanse();
    }
}

#ifdef DEBUG
void Beacon::debugPrint() const
{
    printf("Beacon:");
    if (g_game.game == GC_LABYRINTH)
    {
        switch (beaconType)
        {
            case BEACON_HEAL:
                printf(" (Healing)\n");
                break;
            case BEACON_SPEED:
                printf(" (Speed)\n");
                break;
            case BEACON_CLEANSE:
                printf(" (Cleansing)\n");
                break;
            case BEACON_INVULNERABILITY:
                printf(" (Invulnerability)\n");
                break;
            case BEACON_MANA_BIND:
                printf(" (Binding)\n");
                break;
        }
    }
    else
    {
        switch (beaconType)
        {
            case BEACON_HEAL:
                printf(" (Healing)\n");
                break;
            case BEACON_SPEED:
                printf(" (Haste)\n");
                break;
            case BEACON_CLEANSE:
                printf(" (Cleansing)\n");
                break;
            case BEACON_SHIELD:
                printf(" (Shield)\n");
                break;
            case BEACON_DISCHARGE:
                printf(" (Discharge)\n");
                break;
            case BEACON_PROTECTOR:
                printf(" (Protector)\n");
                break;
            case BEACON_STATIC:
                printf(" (Static)\n");
                break;
            case BEACON_OMNI:
                printf(" (Omni)\n");
                break;
        }
    }
    printf("\tHP: %lf/%lf | Armor: %lf\n", hp, hpMax, armor);
}
#endif
