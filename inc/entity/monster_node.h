#pragma once

#include "common/vector2d.h"

#include "constants/building_type.h"
#include "constants/game_header.h"

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
        : x(x_),
          y(y_),
          tileDirection(g_game.ingameMapHeight, g_game.ingameMapWidth),
          tileDistance(g_game.ingameMapHeight, g_game.ingameMapWidth)
    {
    }

    vector2d<PathWeight> tileDirection;
    vector2d<int> tileDistance;
    BUILDING_TYPE type;
    float x;
    float y;

    static int pickDirection(const PathWeight& path);

#ifdef DEBUG
    void debugDrawPathWeights(IngameCore& core);
#endif
};

class MonsterSpawnNode : public MonsterNode
{
public:
    MonsterSpawnNode(float x_, float y_) : MonsterNode(x_, y_) {}
    float spawnX;
    float spawnY;
};
