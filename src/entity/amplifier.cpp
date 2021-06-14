#include "entity/amplifier.h"
#include "entity/gem.h"

#include "constants/game_header.h"

Amplifier::Amplifier(int ix_, int iy_)
    : Building(ix_, iy_)
{
    type = TILE_AMPLIFIER;
    cooldownTimer = 0;
}

double Amplifier::getGCLAmplifyPower() const
{
    if (g_game.game != GC_LABYRINTH)
        return 1.0;

    int numTargets = 0;
    for (const Amplifiable* a : amplifying)
    {
        // In GCL, Amplifiable in the Amplifier's list can ONLY be a Building.
        const Building* pBuilding = reinterpret_cast<const Building*>(a);
        if (pBuilding->pGem != NULL)
            ++numTargets;
    }

    return 0.3 - 0.03 * numTargets;
}

void Amplifier::insertGem(Gem* pGem_)
{
    Building::insertGem(pGem_);

    if (g_game.game != GC_LABYRINTH)
        cooldownTimer = 10000;

    pGem->recalculateShotData();

    if (!isCoolingDown())
    {
        for (Amplifiable* a : amplifying)
            a->recalculateAmplifyEffects();
    }
}

void Amplifier::removeGem()
{
    Gem* pOldGem = pGem;

    Building::removeGem();

    pOldGem->recalculateShotData();

    for (Amplifiable* a : amplifying)
        a->recalculateAmplifyEffects();
}

void Amplifier::updateGem()
{
    if (g_game.game != GC_LABYRINTH)
        cooldownTimer = 10000;

    for (Amplifiable* a : amplifying)
        a->recalculateAmplifyEffects();
}

void Amplifier::tickCooldown(int frames)
{
    cooldownTimer -= frames * 50;

    if (cooldownTimer <= 0)
    {
        for (Amplifiable* a : amplifying)
            a->recalculateAmplifyEffects();
    }
}
