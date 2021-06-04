#pragma once

#include "common/vector2d.h"

#include "constants/game_header.h"
#include "constants/tile_type.h"

struct IngameLevelDefinition
{
    IngameLevelDefinition() : tiles(g_game.ingameMapHeight, g_game.ingameMapWidth) {}

    vector2d<TILE_TYPE> tiles;
    int orbX, orbY;
};
