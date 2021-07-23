#include "ingame/ingame_map.h"
#include "ingame/ingame_core.h"

#include "wrapfbg.h"

#include <algorithm>
#include <cstdio>

IngameMap::IngameMap(IngameCore& core, IngameLevelDefinition& level)
    : tileOccupied(g_game.ingameMapHeight, g_game.ingameMapWidth),
      tileBuilding(g_game.ingameMapHeight, g_game.ingameMapWidth),
      tileStructure(g_game.ingameMapHeight, g_game.ingameMapWidth),
      manaPool(core.manaPool),
      buildingController(level),
      structureController(manaPool, level),
      enemyController(manaPool),
      projectileController(*this),
      pathfinder(*this, structureController, level)
{
    tileOccupied = level.tiles;

    pSelectedTarget = NULL;

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

    std::vector<MonsterNest>& monsterNests = structureController.getMonsterNests();
    for (int i = 0; i < level.monsterNests.size(); ++i)
    {
        int nx = std::get<0>(level.monsterNests[i]);
        int ny = std::get<1>(level.monsterNests[i]);
        for (int y = ny; y < ny + g_game.ingameMonsterNestSize; ++y)
        {
            for (int x = nx; x < nx + g_game.ingameMonsterNestSize; ++x)
            {
                TILE_TYPE t = level.tiles.at(y, x);
                if (t == TILE_PATH)
                    tileOccupied.at(y, x) = TILE_MONSTER_NEST_PATH;
                else
                    tileOccupied.at(y, x) = TILE_MONSTER_NEST;

                tileStructure.at(y, x) = &monsterNests[i];
            }
        }
    }

    for (const LevelManaShardDefinition& m : level.manaShards)
    {
        placeManaShard(m.x, m.y, m.size, m.mana, m.shell, m.corrupted);
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
    structureController.render(pFbg, window);
    projectileController.render(pFbg, window);

    if (pSelectedTarget != NULL)
    {
        int ix = pSelectedTarget->x * scale + window.x;
        int iy = pSelectedTarget->y * scale + window.y;
        int vertices[8] =
            {
                ix, iy - 8,
                ix + 8, iy,
                ix, iy + 8,
                ix - 8, iy};
        fbg_polygon(pFbg, 4, vertices, 0x10, 0x10, 0x10);
    }

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
        case TILE_SHRINE_CB:
        case TILE_SHRINE_LI:
            bw = bh = g_game.ingameBuildingSize;
            break;
        default:
            return false;
    }

    if ((x + bw > g_game.ingameMapWidth) || (y + bh > g_game.ingameMapHeight))
    {
        printf("Can't build\n");
        return false;
    }

    if (!structureController.checkStaticBeacons(x, y, bw, bh))
    {
        printf("Can't build\n");
        return false;
    }

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
                    {
                        printf("Can't build\n");
                        return false;
                    }
                    break;
                case TILE_PATH:
                    overlapsPath = true;
                    break;
                default:
                    printf("Can't build\n");
                    return false;
            }
        }
    }

    if (!buildingController.hasBuildMana(manaPool, building, 1))
    {
        printf("Not enough mana\n");
        return false;
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
        case TILE_SHRINE_CB:
        case TILE_SHRINE_LI:
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
        case TILE_SHRINE_CB:
            pBuilt = buildingController.addShrine(*this, x, y, SHRINE_CHARGED_BOLTS);
            break;
        case TILE_SHRINE_LI:
            pBuilt = buildingController.addShrine(*this, x, y, SHRINE_LIGHTNING);
            break;
    }

    for (int j = y; j < y + bh; ++j)
    {
        for (int i = x; i < x + bw; ++i)
        {
            TILE_TYPE& t = tileOccupied.at(j, i);
            bool isPath = (t == TILE_PATH) || (t == TILE_WALL_PATH);

            switch (building)
            {
                case TILE_TOWER:
                    overlapsPath |= (t == TILE_PATH);
                    t = isPath ? TILE_TOWER_PATH : TILE_TOWER;
                    break;
                case TILE_AMPLIFIER:
                    overlapsPath |= (t == TILE_PATH);
                    t = isPath ? TILE_AMPLIFIER_PATH : TILE_AMPLIFIER;
                    break;
                case TILE_SHRINE_CB:
                case TILE_SHRINE_LI:
                    overlapsPath |= (t == TILE_PATH);
                    t = building;
                    break;
                case TILE_TRAP:
                    t = TILE_TRAP;
                    break;
                case TILE_ORB:
                    t = TILE_ORB;
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

    if (!structureController.checkStaticBeacons(x1, y1, x2 - x1 + 1, y2 - y1 + 1))
    {
        printf("Can't Build\n");
        return STATUS_INVALID_OPERATION;
    }

    int builtWalls = 0;

    for (int x = x1; x <= x2; ++x)
    {
        for (int y = y1; y <= y2; ++y)
        {
            switch (tileOccupied.at(y, x))
            {
                case TILE_NONE:
                    ++builtWalls;
                    break;
                case TILE_WALL:
                case TILE_WALL_PATH:
                    break;
                case TILE_PATH:
                    ++builtWalls;
                    overlapsPath = true;
                    break;
                default:
                    printf("Can't Build\n");
                    return STATUS_INVALID_OPERATION;
            }
        }
    }

    if (!buildingController.hasBuildMana(manaPool, TILE_WALL, builtWalls))
    {
        printf("Not enough mana\n");
        return STATUS_INVALID_OPERATION;
    }

    if (overlapsPath && pathfinder.checkBlocking(*this, x1, y1, x2 - x1 + 1, y2 - y1 + 1))
    {
        printf("Blocking!\n");
        return STATUS_INVALID_OPERATION;
    }

    buildingController.spendBuildMana(manaPool, TILE_WALL, builtWalls);

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

    buildingController.spendBuildMana(manaPool, TILE_TOWER, 1);
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

    buildingController.spendBuildMana(manaPool, TILE_TRAP, 1);
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

    buildingController.spendBuildMana(manaPool, TILE_AMPLIFIER, 1);
    status = placeBuilding(TILE_AMPLIFIER, x, y);

#ifdef DEBUG
    if (status == STATUS_OK)
        printf("Build Amplifier: %i %i\n", x, y);
#endif

    return status;
}

STATUS IngameMap::buildShrine(int x, int y, SHRINE_TYPE type)
{
    STATUS status = STATUS_OK;
    TILE_TYPE tile;

    switch (type)
    {
        case SHRINE_CHARGED_BOLTS:
            tile = TILE_SHRINE_CB;
            break;
        case SHRINE_LIGHTNING:
            tile = TILE_SHRINE_LI;
            break;
        default:
            return STATUS_INVALID_ARGUMENT;
    }

    if (x < 0 || x >= g_game.ingameMapWidth || y < 0 || y >= g_game.ingameMapHeight)
        return STATUS_INVALID_ARGUMENT;

    if (!verifyBuilding(tile, x, y))
        return STATUS_INVALID_OPERATION;

    buildingController.spendBuildMana(manaPool, tile, 1);
    status = placeBuilding(tile, x, y);

#ifdef DEBUG
    if (status == STATUS_OK)
        printf("Build Shrine (%s): %i %i\n", SHRINE_TYPE_NAME[type], x, y);
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

    TILE_TYPE tile = tileOccupied.at(y, x);
    Building* pBuilding = tileBuilding.at(y, x);
    bool shouldDestroyWalls = false;

    if (pBuilding != NULL)
    {
        if (pBuilding->canBeDemolished())
        {
            // Only CS also demolishes nearby walls when bombing a building
            if (g_game.game == GC_CHASINGSHADOWS)
                shouldDestroyWalls = true;
            destroyBuilding(pBuilding);
        }
    }
    else if ((tile == TILE_WALL) || (tile == TILE_WALL_PATH))
    {
        shouldDestroyWalls = true;
    }

    if (shouldDestroyWalls)
        destroyWalls(x, y);

    return STATUS_OK;
}

void IngameMap::placeManaShard(int x, int y, int size, double mana, double shell, bool corrupted)
{
    ManaShard& shard = structureController.addManaShard(x, y, size, mana, shell, corrupted);

    for (int j = y; j < y + size; ++j)
    {
        for (int i = x; i < x + size; ++i)
        {
            tileOccupied.at(j, i) = TILE_MANA_SHARD;
            tileStructure.at(j, i) = &shard;
        }
    }
}

void IngameMap::placeBeacon(int x, int y)
{
    Beacon& beacon = structureController.addBeacon(x, y);

    for (int j = y; j < y + g_game.ingameBuildingSize; ++j)
    {
        for (int i = x; i < x + g_game.ingameBuildingSize; ++i)
        {
            tileOccupied.at(j, i) = TILE_BEACON;
            tileStructure.at(j, i) = &beacon;
        }
    }
}

void IngameMap::spawnManaShard()
{
    // Can't hash a pair, so use a u16|u16 instead.
    std::unordered_set<uint32_t> spawnLocations;

    // TODO Optimize this - this set can be cached and only updated when something is placed on the map
    // Also, share it with GCCS shrine spawning
    for (uint16_t y = 0; y <= g_game.ingameMapHeight - g_game.ingameBuildingSize; ++y)
    {
        for (uint16_t x = 0; x <= g_game.ingameMapWidth - g_game.ingameBuildingSize; ++x)
        {
            bool canPlace = true;
            for (uint16_t j = y; j < y + g_game.ingameBuildingSize; ++j)
            {
                for (uint16_t i = x; i < x + g_game.ingameBuildingSize; ++i)
                {
                    if (tileOccupied.at(j, i) != TILE_NONE)
                    {
                        canPlace = false;
                        break;
                    }
                }
                if (!canPlace)
                    break;
            }

            if (canPlace)
                spawnLocations.emplace((uint32_t(x) << 16) | uint32_t(y));
        }
    }

    std::unordered_set<uint32_t>::iterator it = spawnLocations.begin();
    std::advance(it, rand() % spawnLocations.size());

    int x = (*it >> 16) & 0xFFFF;
    int y = (*it) & 0xFFFF;

    placeManaShard(x, y, g_game.ingameBuildingSize, 1000.0, 100.0, false);

    spawnLocations.erase(it);
}

void IngameMap::spawnBeacons(int numBeacons)
{
    // Can't hash a pair, so use a u16|u16 instead.
    std::unordered_set<uint32_t> spawnLocations;

    // TODO Optimize this - this set can be cached and only updated when something is placed on the map
    // Also, share it with GCCS shrine spawning
    for (uint16_t y = 0; y <= g_game.ingameMapHeight - g_game.ingameBuildingSize; ++y)
    {
        for (uint16_t x = 0; x <= g_game.ingameMapWidth - g_game.ingameBuildingSize; ++x)
        {
            bool canPlace = true;
            for (uint16_t j = y; j < y + g_game.ingameBuildingSize; ++j)
            {
                for (uint16_t i = x; i < x + g_game.ingameBuildingSize; ++i)
                {
                    if (tileOccupied.at(j, i) != TILE_NONE)
                    {
                        canPlace = false;
                        break;
                    }
                }
                if (!canPlace)
                    break;
            }

            if (canPlace)
                spawnLocations.emplace((uint32_t(x) << 16) | uint32_t(y));
        }
    }

    for (int i = 0; (i < numBeacons) && !spawnLocations.empty(); ++i)
    {
        std::unordered_set<uint32_t>::iterator it = spawnLocations.begin();
        std::advance(it, rand() % spawnLocations.size());

        int x = (*it >> 16) & 0xFFFF;
        int y = (*it) & 0xFFFF;

        placeBeacon(x, y);

        spawnLocations.erase(it);
        if (g_game.ingameBuildingSize > 1)
        {
            for (int j = std::max(0, y - g_game.ingameBuildingSize + 1); j < y + g_game.ingameBuildingSize; ++j)
            {
                for (int i = std::max(0, x - g_game.ingameBuildingSize + 1); i < x + g_game.ingameBuildingSize; ++i)
                {
                    it = spawnLocations.find((uint32_t(i) << 16) | uint32_t(j));
                    if (it != spawnLocations.end())
                        spawnLocations.erase(it);
                }
            }
        }
    }
}

void IngameMap::destroyManaShard(ManaShard* pManaShard)
{
    const int x = pManaShard->ix;
    const int y = pManaShard->iy;
    for (int j = y; j < y + pManaShard->width; ++j)
    {
        for (int i = x; i < x + pManaShard->width; ++i)
        {
            tileOccupied.at(j, i) = TILE_NONE;
            tileStructure.at(j, i) = NULL;
        }
    }
}

void IngameMap::destroyBeacon(Beacon* pBeacon)
{
    const int x = pBeacon->ix;
    const int y = pBeacon->iy;
    for (int j = y; j < y + g_game.ingameBuildingSize; ++j)
    {
        for (int i = x; i < x + g_game.ingameBuildingSize; ++i)
        {
            tileOccupied.at(j, i) = TILE_NONE;
            tileStructure.at(j, i) = NULL;
        }
    }
}

void IngameMap::dropGemBomb(Gem* pGem, float x, float y)
{
    float bombRange = pGem->getBombRange();

    std::vector<Targetable*> targets = enemyController.getShrineTargetsWithinRangeSq(y, x, bombRange * bombRange);
    if (targets.empty())
        return;

    if (g_game.game == GC_LABYRINTH)
    {
        int numHits = 0.85f * pGem->grade + 2.0f;
        double damage = pGem->getBombDamage();
        damage *= 1.0 + pGem->shotFinal.rollCritMultiplier();

        for (int i = 0; (i < numHits) && (i < targets.size()); ++i)
        {
            targets[i]->receiveBombDamage(pGem->shotFinal, damage);
        }
    }
    else
    {
        double damage = pGem->getBombDamage() / targets.size();
        damage *= 1.0 + pGem->shotFinal.rollCritMultiplier();

        for (size_t i = 0; i < targets.size(); ++i)
        {
            targets[i]->receiveBombDamage(pGem->shotFinal, damage);
        }
    }
}

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
            double banishmentCost = monster.sortBanishmentCost *
                                    buildingController.getOrb().getBanishmentCostMultiplierFinal();
            monster.multiplyBanishmentCost(1.68);

            manaPool.addMana(-banishmentCost, false);
            if (manaPool.getMana() < 0)
            {
                buildingController.getOrb().breakOrb();
            }
            else
            {
                if ((monster.pSourceNode == NULL) ||
                    ((monster.pSourceNode->nodeType == TILE_MONSTER_NEST) &&
                        reinterpret_cast<const MonsterNest*>(monster.pSourceNode)->isKilled))
                {
                    const std::vector<const MonsterSpawnNode*> nodes =
                        pathfinder.getMonsterSpawnNodes();
                    monster.pSourceNode = nodes[rand() % nodes.size()];
                }
                monster.spawn();
            }
        }
    }
}

void IngameMap::setSelectedTarget(Targetable* pTarget)
{
    if (pSelectedTarget != NULL)
        pSelectedTarget->isSelectedTarget = false;

    pSelectedTarget = pTarget;

    if (pTarget != NULL)
        pTarget->isSelectedTarget = true;
}
