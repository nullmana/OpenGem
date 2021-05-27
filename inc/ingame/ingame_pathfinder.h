#pragma once

#include "common/vector2d.h"

#include "constants/building_type.h"
#include "constants/status.h"

#include "entity/orb.h"
#include "entity/path_edge_node.h"

#include "ingame/ingame_input_handler.h"
#include "ingame/ingame_level_definition.h"

#include <vector>

class IngameCore;
class IngameMap;
struct IngameLevelDefinition;

class IngamePathfinder
{
private:
    std::vector<PathEdgeNode> pathEdges;
    Orb& orbNode;

    static void calculateDistanceToNode(
        const MonsterNode& node, const vector2d<BUILDING_TYPE>& map, vector2d<int>& distance);

    void recalculateNodePaths(IngameMap& map, MonsterNode* pNode);

public:
    IngamePathfinder(IngameMap& map, IngameLevelDefinition& level);

    void recalculatePaths(IngameMap& map);

    bool checkBlocking(const IngameMap& map, int x, int y, int w, int h);

    std::vector<const MonsterSpawnNode*> getMonsterSpawnNodes() const;
    const Orb* getOrbNode() const { return &orbNode; }

#ifdef DEBUG
    void debugDrawPathWeights(IngameCore& core, int node);
#endif
};
