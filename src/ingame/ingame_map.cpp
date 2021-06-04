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
    tileOccupied = level.tiles;

    for (int y = 0; y < g_game.ingameMapHeight; ++y)
    {
        for (int x = 0; x < g_game.ingameMapWidth; ++x)
        {
            TILE_TYPE b = level.tiles.at(y, x);
            switch (b)
            {
                case TILE_PATH:
                case TILE_WALL_PATH:
                case TILE_WALL:
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
            TILE_TYPE type = tileOccupied.at(j, i);
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

bool IngameMap::verifyBuilding(TILE_TYPE building, int x, int y)
{
    int bw, bh;
    bool overlapsPath = false;

    switch (building)
    {
        case TILE_TOWER:
        case TILE_TRAP:
        case TILE_AMPLIFIER:
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
            TILE_TYPE b = tileOccupied.at(j, i);
            switch (b)
            {
                case TILE_NONE:
                case TILE_WALL:
                case TILE_WALL_PATH:
                    if (building == TILE_TRAP)
                        return false;
                    break;
                case TILE_PATH:
                    overlapsPath = true;
                    break;
                default:
                    return false;
            }
        }
    }

    if ((building != TILE_TRAP) && overlapsPath && pathfinder.checkBlocking(*this, x, y, bw, bh))
    {
        printf("Blocking!\n");
        return false;
    }

    return true;
}

STATUS IngameMap::placeBuilding(TILE_TYPE building, int x, int y)
{
    int bw, bh;
    bool overlapsPath = false;

    switch (building)
    {
        case TILE_TOWER:
        case TILE_TRAP:
        case TILE_AMPLIFIER:
        case TILE_ORB:
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
        case TILE_TOWER:
            pBuilt = &buildingController.addTower(x, y);
            break;
        case TILE_TRAP:
            pBuilt = &buildingController.addTrap(x, y);
            break;
        case TILE_AMPLIFIER:
            pBuilt = &buildingController.addAmplifier(x, y);
            break;
    }

    for (int j = y; j < y + bh; ++j)
    {
        for (int i = x; i < x + bw; ++i)
        {
            TILE_TYPE b = tileOccupied.at(j, i);
            bool isPath = (b == TILE_PATH) || (b == TILE_WALL_PATH);

            switch (building)
            {
                case TILE_TOWER:
                    overlapsPath |= (b == TILE_PATH);
                    tileOccupied.at(j, i) = isPath ? TILE_TOWER_PATH : TILE_TOWER;
                    break;
                case TILE_AMPLIFIER:
                    overlapsPath |= (b == TILE_PATH);
                    tileOccupied.at(j, i) = isPath ? TILE_AMPLIFIER_PATH : TILE_AMPLIFIER;
                    break;
                case TILE_TRAP:
                    tileOccupied.at(j, i) = TILE_TRAP;
                    break;
                case TILE_ORB:
                    tileOccupied.at(j, i) = TILE_ORB;
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
    printf("Placed %s@(%i, %i)\n", TILE_TYPE_NAME[building], x, y);
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
                case TILE_NONE:
                case TILE_WALL:
                case TILE_WALL_PATH:
                    break;
                case TILE_PATH:
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
            if ((tileOccupied.at(y, x) == TILE_PATH) || (tileOccupied.at(y, x) == TILE_WALL_PATH))
                tileOccupied.at(y, x) = TILE_WALL_PATH;
            else
                tileOccupied.at(y, x) = TILE_WALL;
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

    if (!verifyBuilding(TILE_TOWER, x, y))
        return STATUS_INVALID_OPERATION;

    status = placeBuilding(TILE_TOWER, x, y);

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

    if (!verifyBuilding(TILE_TRAP, x, y))
        return STATUS_INVALID_OPERATION;

    status = placeBuilding(TILE_TRAP, x, y);

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

    if (!verifyBuilding(TILE_AMPLIFIER, x, y))
        return STATUS_INVALID_OPERATION;

    status = placeBuilding(TILE_AMPLIFIER, x, y);

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
            TILE_TYPE& t = tileOccupied.at(iy, ix);
            if ((t == TILE_TOWER) || (t == TILE_AMPLIFIER))
            {
                t = TILE_NONE;
            }
            else
            {
                t = TILE_PATH;
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
            TILE_TYPE& t = tileOccupied.at(iy, ix);
            switch (t)
            {
                case TILE_WALL:
                    t = TILE_NONE;
                    break;
                case TILE_WALL_PATH:
                    t = TILE_PATH;
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
