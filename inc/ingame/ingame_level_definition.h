#pragma once

#include "common/vector2d.h"

#include "constants/game_header.h"
#include "constants/tile_type.h"

#include <tuple>
#include <vector>

struct IngameLevelDefinition
{
    IngameLevelDefinition() : tiles(g_game.ingameMapHeight, g_game.ingameMapWidth) {}

    vector2d<TILE_TYPE> tiles;
    std::vector<std::tuple<int, int, bool>> monsterNests;
    int orbX, orbY;
};
