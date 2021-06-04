#include "ingame/ingame_pathfinder.h"
#include "ingame/ingame_core.h"

#include <climits>
#include <cstdio>
#include <queue>
#include <tuple>

IngamePathfinder::IngamePathfinder(IngameMap& map, IngameLevelDefinition& level)
    : orbNode(map.buildingController.getOrb())
{
    for (int x = 0; x < g_game.ingameMapWidth; ++x)
    {
        if (level.tiles.at(0, x) == TILE_PATH)
            pathEdges.emplace_back(x, 0);
        if (level.tiles.at(g_game.ingameMapHeight - 1, x) == TILE_PATH)
            pathEdges.emplace_back(x, g_game.ingameMapHeight - 1);
    }

    for (int y = 1; y < g_game.ingameMapHeight - 1; ++y)
    {
        if (level.tiles.at(y, 0) == TILE_PATH)
            pathEdges.emplace_back(0, y);
        if (level.tiles.at(y, g_game.ingameMapWidth - 1) == TILE_PATH)
            pathEdges.emplace_back(g_game.ingameMapWidth - 1, y);
    }
}

void IngamePathfinder::calculateDistanceToNode(
    const MonsterNode& node, const vector2d<TILE_TYPE>& map, vector2d<int>& distance)
{
    std::queue<std::tuple<int, int>> queue;

    int nx = (int)(node.x - 0.25f);
    int ny = (int)(node.y - 0.25f);

    distance.at(ny, nx) = 0;
    queue.emplace(ny, nx);

    if ((g_game.ingameBuildingSize == 2) &&
        ((nx != (int)(node.x + 0.25f)) || (ny != (int)(node.y + 0.25f))))
    {
        // node is aligned between tiles, start with 2x2
        distance.at(ny + 1, nx) = 0;
        queue.emplace(ny + 1, nx);
        distance.at(ny, nx + 1) = 0;
        queue.emplace(ny, nx + 1);
        distance.at(ny + 1, nx + 1) = 0;
        queue.emplace(ny + 1, nx + 1);
    }

    while (!queue.empty())
    {
        std::tuple<int, int> p = queue.front();
        queue.pop();

        int y = std::get<0>(p);
        int x = std::get<1>(p);
        int d = distance.at(y, x) + 1;

        if ((x > 0) && isPathable(map.at(y, x - 1)) && (distance.at(y, x - 1) > d))
        {
            distance.at(y, x - 1) = d;
            queue.emplace(y, x - 1);
        }
        if ((x < g_game.ingameMapWidth - 1) && isPathable(map.at(y, x + 1)) &&
            (distance.at(y, x + 1) > d))
        {
            distance.at(y, x + 1) = d;
            queue.emplace(y, x + 1);
        }
        if ((y > 0) && isPathable(map.at(y - 1, x)) && (distance.at(y - 1, x) > d))
        {
            distance.at(y - 1, x) = d;
            queue.emplace(y - 1, x);
        }
        if ((y < g_game.ingameMapHeight - 1) && isPathable(map.at(y + 1, x)) &&
            (distance.at(y + 1, x) > d))
        {
            distance.at(y + 1, x) = d;
            queue.emplace(y + 1, x);
        }
    }
}

bool IngamePathfinder::checkBlocking(const IngameMap& map, int x, int y, int w, int h)
{
    const vector2d<TILE_TYPE>& mapTiles = map.getTileOccupiedMap();
    vector2d<int> distance(g_game.ingameMapHeight, g_game.ingameMapWidth, INT_MAX);

    // Create a copy of the map with the would-be blocker applied
    vector2d<TILE_TYPE> mapInstance = mapTiles;

    for (int j = y; j < y + h; ++j)
    {
        for (int i = x; i < x + w; ++i)
        {
            // Early fail if any tiles the building occupies have something on them
            if (map.enemyController.getMonstersOnTile(y, x).size() != 0U)
            {
                return true;
            }

            mapInstance.at(j, i) = TILE_WALL;
        }
    }

    calculateDistanceToNode(orbNode, mapInstance, distance);

    // Check whether any nodes are unreachable - if so, blocking
    for (PathEdgeNode& node : pathEdges)
    {
        if (distance.at(node.iy, node.ix) == INT_MAX)
            return true;
    }

    for (int j = 0; j < g_game.ingameMapHeight; ++j)
    {
        for (int i = 0; i < g_game.ingameMapWidth; ++i)
        {
            if ((mapTiles.at(j, i) == TILE_PATH) &&
                (map.enemyController.getMonstersOnTile(j, i).size() != 0U) &&
                (distance.at(j, i) == INT_MAX))
            {
                return true;
            }
        }
    }

    return false;
}

void IngamePathfinder::recalculateNodePaths(IngameMap& map, MonsterNode* pNode)
{
    vector2d<int>& distance = pNode->tileDistance;
    distance.clear(INT_MAX);
    calculateDistanceToNode(*pNode, map.getTileOccupiedMap(), distance);

    int weight_cardinal = 1;
    int weight_diagonalEqual = 0;
    int weight_diagonalCloser = 1;

    if (g_game.game == GC_LABYRINTH)
    {
        weight_cardinal = 1;
        weight_diagonalEqual = 0;
        weight_diagonalCloser = 1;
    }
    else if (g_game.game == GC_CHASINGSHADOWS)
    {
        weight_cardinal = 12;
        weight_diagonalEqual = 1;
        weight_diagonalCloser = 4;
    }

    for (int y = 0; y < g_game.ingameMapHeight; ++y)
    {
        for (int x = 0; x < g_game.ingameMapWidth; ++x)
        {
            bool N = false, E = false, S = false, W = false;
            int current = distance.at(y, x);
            int diff;
            PathWeight& tile = pNode->tileDirection.at(y, x);
            tile.clear();

            if (current == INT_MAX)
                continue;

            // Cardinal directions
            if (y > 0)
            {
                diff = distance.at(y - 1, x) - current;
                if (diff < 2)
                {
                    // If accessible, mark as valid for diagonals
                    N = true;
                    // If strictly closer to orb, path towards that direction
                    if (diff < 0)
                        tile.set(0, weight_cardinal);
                }
            }
            if (y < g_game.ingameMapHeight - 1)
            {
                diff = distance.at(y + 1, x) - current;
                if (diff < 2)
                {
                    S = true;
                    if (diff < 0)
                        tile.set(4, weight_cardinal);
                }
            }
            if (x > 0)
            {
                diff = distance.at(y, x - 1) - current;
                if (diff < 2)
                {
                    W = true;
                    if (diff < 0)
                        tile.set(6, weight_cardinal);
                }
            }
            if (x < g_game.ingameMapWidth - 1)
            {
                diff = distance.at(y, x + 1) - current;
                if (diff < 2)
                {
                    E = true;
                    if (diff < 0)
                        tile.set(2, weight_cardinal);
                }
            }

            // Diagonals
            // Only follow diagonals if further than 8 tiles from source in GCCS, because...
            // reasons.
            if ((g_game.game == GC_LABYRINTH) || (current > 8))
            {
                if (y > 0)
                {
                    if (x > 0)
                    {
                        diff = distance.at(y - 1, x - 1) - current;
                        // Weigh more heavily if heading towards orb, but occasionally shuffle
                        // around
                        if (N && W && (diff < 1))
                            tile.set(7, (diff < 0) ? weight_diagonalCloser : weight_diagonalEqual);
                    }
                    if (x < g_game.ingameMapWidth - 1)
                    {
                        diff = distance.at(y - 1, x + 1) - current;
                        if (N && E && (diff < 1))
                            tile.set(1, (diff < 0) ? weight_diagonalCloser : weight_diagonalEqual);
                    }
                }
                if (y < g_game.ingameMapHeight - 1)
                {
                    if (x > 0)
                    {
                        diff = distance.at(y + 1, x - 1) - current;
                        if (S && W && (diff < 1))
                            tile.set(5, (diff < 0) ? weight_diagonalCloser : weight_diagonalEqual);
                    }
                    if (x < g_game.ingameMapWidth - 1)
                    {
                        diff = distance.at(y + 1, x + 1) - current;
                        if (S && E && (diff < 1))
                            tile.set(3, (diff < 0) ? weight_diagonalCloser : weight_diagonalEqual);
                    }
                }
            }
        }
    }
}

void IngamePathfinder::recalculatePaths(IngameMap& map)
{
    recalculateNodePaths(map, &map.buildingController.getOrb());
    for (PathEdgeNode& node : pathEdges)
        recalculateNodePaths(map, &node);
}

std::vector<const MonsterSpawnNode*> IngamePathfinder::getMonsterSpawnNodes() const
{
    std::vector<const MonsterSpawnNode*> nodes;

    for (const PathEdgeNode& p : pathEdges)
    {
        nodes.push_back(&p);
    }

    return nodes;
}

#ifdef DEBUG
void IngamePathfinder::debugDrawPathWeights(IngameCore& core, int node)
{
    static int prev = INT_MAX;
    if (prev != node)
    {
        printf("Debug draw: %i\n", node);
        prev = node;
    }

    if (node == -1)
        core.map.buildingController.getOrb().debugDrawPathWeights(core);
    else if (node >= 0 && node < pathEdges.size())
        pathEdges[node].debugDrawPathWeights(core);
}
#endif
