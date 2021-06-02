#include "ingame/ingame_map.h"
#include "ingame/ingame_core.h"

#include "wrapfbg.h"

#include <algorithm>
#include <cstdio>

IngameMap::IngameMap(IngameCore& core, IngameLevelDefinition& level)
    : tileOccupied(g_game.ingameMapHeight, g_game.ingameMapWidth),
      tileBuilding(g_game.ingameMapHeight, g_game.ingameMapWidth),
      manaPool(core.manaPool),
      buildingController(level),
      pathfinder(*this, level)
{
    tileOccupied = level.buildings;

    for (int y = 0; y < g_game.ingameMapHeight; ++y)
    {
        for (int x = 0; x < g_game.ingameMapWidth; ++x)
        {
            BUILDING_TYPE b = level.buildings.at(y, x);
            switch (b)
            {
                case BUILDING_PATH:
                case BUILDING_WALL_PATH:
                case BUILDING_WALL:
                    break;
                default:
                    placeBuilding(b, x, y);
                    break;
            }
        }
    }

    pathfinder.recalculatePaths(*this);
}

STATUS IngameMap::render(struct _fbg* pFbg, const Window& window) const
{
    float scale = window.width / float(g_game.ingameMapWidth);

    for (int j = 0; j < g_game.ingameMapHeight; ++j)
    {
        for (int i = 0; i < g_game.ingameMapWidth; ++i)
        {
            BUILDING_TYPE type = tileOccupied.at(j, i);
            fbg_rect(pFbg, window.x + i * scale, window.y + j * scale, scale, scale,
                (TILE_COLOR[type] >> 16) & 0xFF, (TILE_COLOR[type] >> 8) & 0xFF,
                (TILE_COLOR[type]) & 0xFF);
        }
    }

    buildingController.render(pFbg, window);
    enemyController.render(pFbg, window);
    projectileController.render(pFbg, window);

    return STATUS_OK;
}

bool IngameMap::verifyBuilding(BUILDING_TYPE building, int x, int y)
{
    int bw, bh;
    bool overlapsPath = false;

    switch (building)
    {
        case BUILDING_TOWER:
        case BUILDING_TRAP:
        case BUILDING_AMPLIFIER:
            bw = bh = g_game.ingameBuildingSize;
            break;
        default:
            return false;
    }

    if ((x + bw > g_game.ingameMapWidth) || (y + bh > g_game.ingameMapHeight))
        return false;

    for (int j = y; j < y + bh; ++j)
    {
        for (int i = x; i < x + bw; ++i)
        {
            BUILDING_TYPE b = tileOccupied.at(j, i);
            switch (b)
            {
                case BUILDING_NONE:
                case BUILDING_WALL:
                case BUILDING_WALL_PATH:
                    if (building == BUILDING_TRAP)
                        return false;
                    break;
                case BUILDING_PATH:
                    overlapsPath = true;
                    break;
                default:
                    return false;
            }
        }
    }

    if ((building != BUILDING_TRAP) && overlapsPath &&
        pathfinder.checkBlocking(*this, x, y, bw, bh))
    {
        printf("Blocking!\n");
        return false;
    }

    return true;
}

STATUS IngameMap::placeBuilding(BUILDING_TYPE building, int x, int y)
{
    int bw, bh;
    bool overlapsPath = false;

    switch (building)
    {
        case BUILDING_TOWER:
        case BUILDING_TRAP:
        case BUILDING_AMPLIFIER:
        case BUILDING_ORB:
            bw = bh = g_game.ingameBuildingSize;
            break;
        default:
            return STATUS_INVALID_ARGUMENT;
    }

    if (x + bw > g_game.ingameMapWidth || y + bh > g_game.ingameMapHeight)
        return STATUS_INVALID_OPERATION;

    Building* pBuilt = NULL;
    switch (building)
    {
        case BUILDING_TOWER:
            pBuilt = &buildingController.addTower(x, y);
            break;
        case BUILDING_TRAP:
            pBuilt = &buildingController.addTrap(x, y);
            break;
        case BUILDING_AMPLIFIER:
            pBuilt = &buildingController.addAmplifier(x, y);
            break;
    }

    for (int j = y; j < y + bh; ++j)
    {
        for (int i = x; i < x + bw; ++i)
        {
            BUILDING_TYPE b = tileOccupied.at(j, i);
            bool isPath = (b == BUILDING_PATH) || (b == BUILDING_WALL_PATH);

            switch (building)
            {
                case BUILDING_TOWER:
                    overlapsPath |= (b == BUILDING_PATH);
                    tileOccupied.at(j, i) = isPath ? BUILDING_TOWER_PATH : BUILDING_TOWER;
                    break;
                case BUILDING_AMPLIFIER:
                    overlapsPath |= (b == BUILDING_PATH);
                    tileOccupied.at(j, i) = isPath ? BUILDING_AMPLIFIER_PATH : BUILDING_AMPLIFIER;
                    break;
                case BUILDING_TRAP:
                    tileOccupied.at(j, i) = BUILDING_TRAP;
                    break;
                case BUILDING_ORB:
                    tileOccupied.at(j, i) = BUILDING_ORB;
                    break;
            }

            tileBuilding.at(j, i) = pBuilt;
        }
    }

    if (overlapsPath)
    {
        pathfinder.recalculatePaths(*this);
        enemyController.forceRepath(x, y, bw, bh);
    }

#ifdef DEBUG
    printf("Placed %s@(%i, %i)\n", BUILDING_TYPE_NAME[building], x, y);
#endif

    return STATUS_OK;
}

STATUS IngameMap::buildWall(int x1, int y1, int x2, int y2)
{
    bool overlapsPath = false;

    if ((x1 < 0) || (x1 >= g_game.ingameMapWidth) || (y1 < 0) || (y1 >= g_game.ingameMapHeight) ||
        (x2 < 0) || (x2 >= g_game.ingameMapWidth) || (y2 < 0) || (y2 >= g_game.ingameMapHeight))
    {
        return STATUS_INVALID_ARGUMENT;
    }

    if (x1 > x2)
        std::swap<int>(x1, x2);
    if (y1 > y2)
        std::swap<int>(y1, y2);

    for (int x = x1; x <= x2; ++x)
    {
        for (int y = y1; y <= y2; ++y)
        {
            switch (tileOccupied.at(y, x))
            {
                case BUILDING_NONE:
                case BUILDING_WALL:
                case BUILDING_WALL_PATH:
                    break;
                case BUILDING_PATH:
                    overlapsPath = true;
                    break;
                default:
                    printf("Can't Build\n");
                    return STATUS_INVALID_OPERATION;
            }
        }
    }

    if (overlapsPath && pathfinder.checkBlocking(*this, x1, y1, x2 - x1 + 1, y2 - y1 + 1))
    {
        printf("Blocking!\n");
        return STATUS_INVALID_OPERATION;
    }

    for (int x = x1; x <= x2; ++x)
    {
        for (int y = y1; y <= y2; ++y)
        {
            if ((tileOccupied.at(y, x) == BUILDING_PATH) ||
                (tileOccupied.at(y, x) == BUILDING_WALL_PATH))
                tileOccupied.at(y, x) = BUILDING_WALL_PATH;
            else
                tileOccupied.at(y, x) = BUILDING_WALL;
        }
    }

    if (overlapsPath)
    {
        pathfinder.recalculatePaths(*this);
        enemyController.forceRepath(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
    }

#ifdef DEBUG
    printf("Build Wall: (%i %i) to (%i %i)\n", x1, y1, x2, y2);
#endif

    return STATUS_OK;
}

STATUS IngameMap::buildTower(int x, int y)
{
    STATUS status = STATUS_OK;

    if (x < 0 || x >= g_game.ingameMapWidth || y < 0 || y >= g_game.ingameMapHeight)
        return STATUS_INVALID_ARGUMENT;

    if (!verifyBuilding(BUILDING_TOWER, x, y))
        return STATUS_INVALID_OPERATION;

    status = placeBuilding(BUILDING_TOWER, x, y);

#ifdef DEBUG
    if (status == STATUS_OK)
        printf("Build Tower: %i %i\n", x, y);
#endif

    return status;
}

STATUS IngameMap::buildTrap(int x, int y)
{
    STATUS status = STATUS_OK;

    if (x < 0 || x >= g_game.ingameMapWidth || y < 0 || y >= g_game.ingameMapHeight)
        return STATUS_INVALID_ARGUMENT;

    if (!verifyBuilding(BUILDING_TRAP, x, y))
        return STATUS_INVALID_OPERATION;

    status = placeBuilding(BUILDING_TRAP, x, y);

#ifdef DEBUG
    if (status == STATUS_OK)
        printf("Build Trap: %i %i\n", x, y);
#endif

    return status;
}

STATUS IngameMap::buildAmplifier(int x, int y)
{
    STATUS status = STATUS_OK;

    if (x < 0 || x >= g_game.ingameMapWidth || y < 0 || y >= g_game.ingameMapHeight)
        return STATUS_INVALID_ARGUMENT;

    if (!verifyBuilding(BUILDING_AMPLIFIER, x, y))
        return STATUS_INVALID_OPERATION;

    status = placeBuilding(BUILDING_AMPLIFIER, x, y);

#ifdef DEBUG
    if (status == STATUS_OK)
        printf("Build Amplifier: %i %i\n", x, y);
#endif

    return status;
}

void IngameMap::destroyBuilding(Building* pBuilding)
{
    bool addedPath = false;
    int ix, iy;

    for (iy = pBuilding->iy; iy < pBuilding->iy + g_game.ingameBuildingSize; ++iy)
    {
        for (ix = pBuilding->ix; ix < pBuilding->ix + g_game.ingameBuildingSize; ++ix)
        {
            BUILDING_TYPE& t = tileOccupied.at(iy, ix);
            if ((t == BUILDING_TOWER) || (t == BUILDING_AMPLIFIER))
            {
                t = BUILDING_NONE;
            }
            else
            {
                t = BUILDING_PATH;
                addedPath = true;
            }

            tileBuilding.at(iy, ix) = NULL;
        }
    }

    ix = pBuilding->ix;
    iy = pBuilding->iy;

    buildingController.destroyBuilding(pBuilding);
    pBuilding = NULL;

    if (addedPath)
    {
        pathfinder.recalculatePaths(*this);
        enemyController.forceRepath(ix, iy, g_game.ingameBuildingSize, g_game.ingameBuildingSize);
    }

#ifdef DEBUG
    printf("Destroy Building: %i %i\n", ix, iy);
#endif
}

void IngameMap::destroyWalls(int x, int y)
{
    const int wallDestroyRadius = g_game.ingameBuildingSize - 1;
    bool addedPath = false;

    for (int iy = std::max(0, y - wallDestroyRadius);
         iy <= std::min(g_game.ingameMapHeight - 1, y + wallDestroyRadius); ++iy)
    {
        for (int ix = std::max(0, x - wallDestroyRadius);
             ix <= std::min(g_game.ingameMapWidth - 1, x + wallDestroyRadius); ++ix)
        {
            BUILDING_TYPE& t = tileOccupied.at(iy, ix);
            switch (t)
            {
                case BUILDING_WALL:
                    t = BUILDING_NONE;
                    break;
                case BUILDING_WALL_PATH:
                    t = BUILDING_PATH;
                    addedPath = true;
                    break;
                default:
                    // Do nothing to other buildings
                    break;
            }
        }
    }

    if (addedPath)
    {
        pathfinder.recalculatePaths(*this);
        enemyController.forceRepath(x - wallDestroyRadius, y - wallDestroyRadius,
            2 * wallDestroyRadius + 1, 2 * wallDestroyRadius + 1);
    }
}

STATUS IngameMap::demolishBuilding(int x, int y)
{
    if (x < 0 || x >= g_game.ingameMapWidth || y < 0 || y >= g_game.ingameMapHeight)
        return STATUS_INVALID_ARGUMENT;

    Building* pBuilding = tileBuilding.at(y, x);
    bool shouldDestroyWalls = true;

    if (pBuilding != NULL)
    {
        if (pBuilding->pGem == NULL)
        {
            // Only CS also demolishes nearby walls when bombing a building
            if (g_game.game != GC_CHASINGSHADOWS)
                shouldDestroyWalls = false;
            destroyBuilding(pBuilding);
        }
        else
        {
            if (g_game.game != GC_CHASINGSHADOWS)
                shouldDestroyWalls = false;
        }
    }

    if (shouldDestroyWalls)
        destroyWalls(x, y);

    return STATUS_OK;
}

void IngameMap::dropGemBomb(Gem* pGem, float x, float y) {}

void IngameMap::monsterReachesTarget(Monster& monster)
{
    if (monster.pTargetNode == &buildingController.getOrb())
    {
        if (buildingController.getOrb().isBroken())
        {
            monster.pickNextTarget();
            return;
        }

        if (monster.incomingShots > 0)
        {
            if (g_game.game == GC_LABYRINTH)
            {
                std::unordered_set<Targetable*> monsterSet;
                monsterSet.insert(&monster);
                projectileController.clearShotsFromTarget(monsterSet);
            }
            else
            {
                projectileController.warpShotsToTarget(&monster);
            }
        }
        if (!monster.isKilled)
        {
            manaPool.addMana(-monster.banishmentCost, false);
            if (manaPool.getMana() < 0)
            {
                buildingController.getOrb().breakOrb();
            }
            else
            {
                monster.spawn();
            }
        }
    }
}
