#pragma once

#include "common/vector2d.h"

#include "constants/game_header.h"
#include "constants/tile_type.h"

#include <cstring>

#ifdef DEBUG
class IngameCore;
#endif

struct PathWeight
{
    // n, ne, e, se, s, sw, w, nw;
    uint8_t weight[8];
    uint16_t sum;

    const static int dx[8];
    const static int dy[8];

    inline void clear() { memset(this, 0, sizeof(PathWeight)); }
    inline void set(uint8_t direction, uint8_t value)
    {
        weight[direction] = value;
        sum += value;
    }
};

class MonsterNode
{
public:
    MonsterNode(float x_, float y_)
        : nodeX(x_),
          nodeY(y_),
          tileDirection(g_game.ingameMapHeight, g_game.ingameMapWidth),
          tileDistance(g_game.ingameMapHeight, g_game.ingameMapWidth)
    {
    }

    vector2d<PathWeight> tileDirection;
    vector2d<int> tileDistance;
    TILE_TYPE nodeType;
    float nodeX;
    float nodeY;

    static int pickDirection(const PathWeight& path);

#ifdef DEBUG
    void debugDrawPathWeights(IngameCore& core);
#endif
};

class MonsterSpawnNode : public MonsterNode
{
public:
    MonsterSpawnNode(float x_, float y_)
        : MonsterNode(x_, y_) {}
    float spawnX;
    float spawnY;
};
