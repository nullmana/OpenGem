#include "entity/building.h"

#include "constants/game_header.h"

Building::Building(int ix_, int iy_) : ix(ix_), iy(iy_)
{
    x = ix_ + 0.5f * g_game.ingameBuildingSize;
    y = iy_ + 0.5f * g_game.ingameBuildingSize;

    pGem = NULL;
}
