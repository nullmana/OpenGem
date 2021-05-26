#pragma once

#include "common/vector2d.h"

#include "constants/building_type.h"
#include "constants/status.h"

#include "entity/orb.h"

#include "ingame/ingame_building_controller.h"
#include "ingame/ingame_enemy_controller.h"
#include "ingame/ingame_input_handler.h"
#include "ingame/ingame_level_definition.h"
#include "ingame/ingame_pathfinder.h"
#include "ingame/ingame_projectile_controller.h"

#include "interface/window.h"

class IngameCore;
class Building;
struct _fbg;

class IngameMap
{
private:
    vector2d<BUILDING_TYPE> tileOccupied;
    vector2d<Building*> tileBuilding;

    bool verifyBuilding(BUILDING_TYPE building, int x, int y);
    STATUS placeBuilding(BUILDING_TYPE building, int x, int y);

public:
    IngameMap(IngameLevelDefinition& level);

    STATUS render(struct _fbg* pFbg, const Window& window) const;

    STATUS buildWall(int x1, int y1, int x2, int y2);
    STATUS buildTower(int x, int y);
    STATUS buildTrap(int x, int y);
    STATUS destroyStructure(int x, int y);

    const vector2d<BUILDING_TYPE>& getTileOccupiedMap() const { return tileOccupied; }

    void monsterReachesTarget(Monster& monster);

    IngameBuildingController buildingController;
    IngameEnemyController enemyController;
    IngameProjectileController projectileController;
    IngamePathfinder pathfinder;
    Orb orb;
};
