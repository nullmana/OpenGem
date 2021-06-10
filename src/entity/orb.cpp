#include "entity/orb.h"
#include "entity/amplifier.h"
#include "entity/gem.h"

#include <cmath>

Orb::Orb(int ix, int iy)
    : MonsterNode(ix + 0.5f * g_game.ingameBuildingSize, iy + 0.5f * g_game.ingameBuildingSize),
      Amplifiable(ix, iy)
{
    nodeType = TILE_ORB;
    broken = false;
}

void Orb::recalculateAmplifyEffects()
{
    banishmentCostReductionAmplifiersRaw = 0.0;

    for (Amplifiable* a : amplifying)
    {
        Amplifier* pAmplifier = reinterpret_cast<Amplifier*>(a);
        if (pAmplifier->pGem != NULL)
        {
            banishmentCostReductionAmplifiersRaw +=
                pow(pAmplifier->pGem->getShotDataComponents().damageMin, 1.21);
        }
    }
}
