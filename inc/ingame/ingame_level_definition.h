#pragma once

#include "common/vector2d.h"

#include "constants/building_type.h"
#include "constants/game_header.h"

struct IngameLevelDefinition
{
    IngameLevelDefinition() : buildings(g_game.ingameMapHeight, g_game.ingameMapWidth) {}

    vector2d<BUILDING_TYPE> buildings;
    int orbX, orbY;
};
