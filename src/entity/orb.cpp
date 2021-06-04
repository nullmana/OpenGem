#include "entity/orb.h"

Orb::Orb(int ix, int iy)
    : MonsterNode(ix + 0.5f * g_game.ingameBuildingSize, iy + 0.5f * g_game.ingameBuildingSize),
      Amplifiable(ix, iy)
{
    type = TILE_ORB;
    broken = false;
}
