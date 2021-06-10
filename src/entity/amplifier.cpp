#include "entity/amplifier.h"
#include "entity/gem.h"

#include "constants/game_header.h"

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

    pGem->recalculateShotData();

    for (Amplifiable* a : amplifying)
        a->recalculateAmplifyEffects();
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
    for (Amplifiable* a : amplifying)
        a->recalculateAmplifyEffects();
}
