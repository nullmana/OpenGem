#pragma once

#include "common/vector2d.h"

#include "constants/shrine_type.h"
#include "constants/status.h"
#include "constants/tile_type.h"

#include "ingame/ingame_building_controller.h"
#include "ingame/ingame_enemy_controller.h"
#include "ingame/ingame_input_handler.h"
#include "ingame/ingame_level_definition.h"
#include "ingame/ingame_pathfinder.h"
#include "ingame/ingame_projectile_controller.h"
#include "ingame/ingame_structure_controller.h"

#include "interface/window.h"

class IngameManaPool;
struct _fbg;

class IngameMap
{
private:
    IngameManaPool& manaPool;
    vector2d<TILE_TYPE> tileOccupied;
    vector2d<Building*> tileBuilding;
    vector2d<Structure*> tileStructure;

    Targetable* pSelectedTarget;

    bool verifyBuilding(TILE_TYPE building, int x, int y);
    STATUS placeBuilding(TILE_TYPE building, int x, int y);
    void destroyBuilding(Building* pBuilding);
    void destroyWalls(int x, int y);
    void placeBeacon(int x, int y);

public:
    IngameMap(IngameCore& core, IngameLevelDefinition& level);

    STATUS render(struct _fbg* pFbg, const Window& window) const;

    STATUS buildWall(int x1, int y1, int x2, int y2);
    STATUS buildTower(int x, int y);
    STATUS buildTrap(int x, int y);
    STATUS buildAmplifier(int x, int y);
    STATUS buildShrine(int x, int y, SHRINE_TYPE type);
    STATUS demolishBuilding(int x, int y);

    void spawnBeacons(int numBeacons);
    void destroyBeacon(Beacon* pBeacon);

    void dropGemBomb(Gem* pGem, float x, float y);

    Building* getBuilding(int y, int x) const { return tileBuilding.at(y, x); }
    Structure* getStructure(int y, int x) const { return tileStructure.at(y, x); }

    const vector2d<TILE_TYPE>& getTileOccupiedMap() const { return tileOccupied; }

    void monsterReachesTarget(Monster& monster);

    Targetable* getSelectedTarget() const { return pSelectedTarget; }
    void setSelectedTarget(Targetable* pTarget);

    IngameBuildingController buildingController;
    IngameStructureController structureController;
    IngameEnemyController enemyController;
    IngameProjectileController projectileController;
    IngamePathfinder pathfinder;
    IngameManaPool& getManaPool() { return manaPool; }
};
