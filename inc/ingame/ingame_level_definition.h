#pragma once

#include "common/vector2d.h"

#include "constants/game_header.h"
#include "constants/tile_type.h"

#include <tuple>
#include <vector>

struct LevelManaShardDefinition
{
    LevelManaShardDefinition(int x_, int y_, int size_, double mana_, double shell_, bool corrupted_)
        : x(x_), y(y_), size(size_), mana(mana_), shell(shell_), corrupted(corrupted_) {}
    int x;
    int y;
    int size;
    double mana;
    double shell;
    bool corrupted;
};

struct IngameLevelDefinition
{
    IngameLevelDefinition()
        : tiles(g_game.ingameMapHeight, g_game.ingameMapWidth) {}

    vector2d<TILE_TYPE> tiles;
    std::vector<std::tuple<int, int, bool>> monsterNests;
    std::vector<LevelManaShardDefinition> manaShards;
    int orbX, orbY;
};
