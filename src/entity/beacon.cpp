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
    }
    else
    {
        range = 50.0f / 17.0f + 60.0f / 17.0f * (rand() / float(RAND_MAX));
    }
}

bool Beacon::tick(IngameMap& map, int frames)
{
    if (isKilled)
        return true;

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
