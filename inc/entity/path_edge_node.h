#pragma once

#include "constants/game_header.h"

#include "entity/monster_node.h"

class PathEdgeNode : public MonsterSpawnNode
{
public:
    PathEdgeNode(int ix_, int iy_) : MonsterSpawnNode(ix_ + 0.5f, iy_ + 0.5f), ix(ix_), iy(iy_)
    {
        spawnX = x;
        spawnY = y;

        if (ix_ == 0)
            spawnX -= 5.88f;
        else if (ix_ == g_game.ingameMapWidth - 1)
            spawnX += 5.88f;
        else if (iy_ == 0)
            spawnY -= 5.88f;
        else
            spawnY += 5.88f;

        type = TILE_PATH;
    }

    int ix;
    int iy;
};
