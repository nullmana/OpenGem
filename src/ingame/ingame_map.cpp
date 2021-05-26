#include "ingame/ingame_map.h"
#include "ingame/ingame_core.h"

#include "wrapfbg.h"

#include <algorithm>
#include <cstdio>

IngameMap::IngameMap(IngameLevelDefinition& level)
    : tileOccupied(g_game.ingameMapHeight, g_game.ingameMapWidth),
      tileBuilding(g_game.ingameMapHeight, g_game.ingameMapWidth),
      orb(level.orbX, level.orbY),
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
                (TILE_COLOR[type] >> 4) & 0xFF, (TILE_COLOR[type] >> 2) & 0xFF,
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
    }

    for (int j = y; j < y + bh; ++j)
    {
        for (int i = x; i < x + bw; ++i)
        {
            BUILDING_TYPE b = tileOccupied.at(j, i);
            bool isPath     = (b == BUILDING_PATH) || (b == BUILDING_WALL_PATH);

            switch (building)
            {
                case BUILDING_TOWER:
                    overlapsPath |= (b == BUILDING_PATH);
                    tileOccupied.at(j, i) = isPath ? BUILDING_TOWER_PATH : BUILDING_TOWER;
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
    STATUS status     = STATUS_OK;
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
    if (status == STATUS_OK)
        printf("Build Wall: (%i %i) to (%i %i)\n", x1, y1, x2, y2);
#endif

    return status;
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

STATUS IngameMap::destroyStructure(int x, int y)
{
    STATUS status  = STATUS_OK;
    bool addedPath = false;

    if (x < 0 || x >= g_game.ingameMapWidth || y < 0 || y >= g_game.ingameMapHeight)
        return STATUS_INVALID_ARGUMENT;

    Building* pBuilt = tileBuilding.at(y, x);

    if (pBuilt != NULL)
    {
        for (int iy = pBuilt->iy; iy < pBuilt->iy + g_game.ingameBuildingSize; ++iy)
        {
            for (int ix = pBuilt->ix; ix < pBuilt->ix + g_game.ingameBuildingSize; ++ix)
            {
                BUILDING_TYPE& t = tileOccupied.at(iy, ix);
                if (t == BUILDING_TOWER)
                {
                    t = BUILDING_NONE;
                }
                else
                {
                    t         = BUILDING_PATH;
                    addedPath = true;
                }

                tileBuilding.at(iy, ix) = NULL;
            }
        }

        buildingController.destroyBuilding(pBuilt);
        pBuilt = NULL;
    }
    else
    {
        BUILDING_TYPE& t = tileOccupied.at(y, x);
        switch (t)
        {
            case BUILDING_WALL:
                t = BUILDING_NONE;
                break;
            case BUILDING_WALL_PATH:
                t = BUILDING_PATH;
                break;
            default:
                status = STATUS_INVALID_OPERATION;
                break;
        }
    }

    if (addedPath)
    {
        pathfinder.recalculatePaths(*this);
        enemyController.forceRepath(x, y, g_game.ingameBuildingSize, g_game.ingameBuildingSize);
    }

#ifdef DEBUG
    if (status == STATUS_OK)
        printf("Destroy Structure: %i %i\n", x, y);
#endif

    return STATUS_OK;
}

void IngameMap::monsterReachesTarget(Monster& monster)
{
    if (monster.pTargetNode == &orb)
    {
        if (monster.incomingShots > 0)
            projectileController.warpShotsToTarget(&monster);
        if (!monster.isKilled)
        {
            monster.spawn();
        }
    }
}
