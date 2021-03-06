#include "ingame/ingame_core.h"

#include <cstdio>
#include <cstring>

#include "wrapfbg.h"

bool bKeepRunning = true;

GameHeader g_game;

int main(int argc, char* argv[])
{
    if ((argc >= 2) && !strcmp(argv[1], "cs"))
    {
        g_game.game = GC_CHASINGSHADOWS;
        g_game.ingameMapWidth = GCCS_INGAME_MAP_WIDTH;
        g_game.ingameMapHeight = GCCS_INGAME_MAP_HEIGHT;
        g_game.ingameBuildingSize = GCCS_INGAME_BUILDING_SIZE;
        g_game.ingameMonsterNestSize = GCCS_INGAME_MONSTER_NEST_SIZE;
        g_game.ingameFramerate = GCCS_INGAME_FRAMERATE;
    }
    else
    {
        g_game.game = GC_LABYRINTH;
        g_game.ingameMapWidth = GCL_INGAME_MAP_WIDTH;
        g_game.ingameMapHeight = GCL_INGAME_MAP_HEIGHT;
        g_game.ingameBuildingSize = GCL_INGAME_BUILDING_SIZE;
        g_game.ingameMonsterNestSize = GCL_INGAME_MONSTER_NEST_SIZE;
        g_game.ingameFramerate = GCL_INGAME_FRAMERATE;
    }

    switch (g_game.game)
    {
        case GC_LABYRINTH:
            printf("Selected Game: Labyrinth\n");
            break;
        case GC_CHASINGSHADOWS:
            printf("Selected Game: Chasing Shadows\n");
            break;
        default:
            printf("Error: Unknown Game: %d\n", g_game.game);
            return -1;
    }

    struct _fbg* pFbg = fbg_glfwSetup(1280, 720, 4, "opengem", 0, 0);
    if (pFbg == NULL)
        return -1;

    // Initialize level
    IngameLevelDefinition level;

    level.orbX = g_game.ingameMapWidth / 2;
    level.orbY = g_game.ingameMapHeight / 2;
    // Horizontal stripe
    for (int y = g_game.ingameMapHeight / 4; y < g_game.ingameMapHeight * 3 / 4; ++y)
        for (int x = 3; x < g_game.ingameMapWidth; ++x)
            level.tiles.at(y, x) = TILE_PATH;
    // Vertical stripe top half
    for (int y = 0; y < g_game.ingameMapHeight / 4; ++y)
        for (int x = g_game.ingameMapWidth * 3 / 8; x < g_game.ingameMapWidth * 5 / 8; ++x)
            level.tiles.at(y, x) = TILE_PATH;
    // Vertical strip bottom half
    for (int y = g_game.ingameMapHeight * 3 / 4; y < g_game.ingameMapHeight; ++y)
        for (int x = g_game.ingameMapWidth * 3 / 8; x < g_game.ingameMapWidth * 5 / 8; ++x)
            level.tiles.at(y, x) = TILE_PATH;

    level.monsterNests.emplace_back(2, (g_game.ingameMapHeight + g_game.ingameMonsterNestSize) / 2, true);
    level.monsterNests.emplace_back(2, (g_game.ingameMapHeight - 2 * g_game.ingameMonsterNestSize) / 2, false);

    level.manaShards.emplace_back(1, 1, g_game.ingameBuildingSize, 1000.0, 0.0, false);
    level.manaShards.emplace_back(3, 1, g_game.ingameBuildingSize + 1, 4000.0, 1000.0, false);
    level.manaShards.emplace_back(g_game.ingameMapWidth - 7, 1, 2 * g_game.ingameBuildingSize + 1, 0.0, 0.0, true);

    level.tiles.at(level.orbY, level.orbX) = TILE_ORB;

    // TODO populate from stage definitions
    level.stage.gemTypes = (1 << GEM_POISON) | (1 << GEM_CRITICAL);
    StageMonsterData& md = level.stage.monsterData;
    md.numWaves = 8;
    md.hpInitial = 4.0;
    md.armorIncrement = 0.1;
    md.typeFrequent = -1;
    md.typeUnused = -1;

    md.numGiantWaves = 1;
    md.numSwarmlingWaves = 1;
    md.firstNonReaverWave = 3;
    md.firstBuffedWave = -1;
    md.buffValueIncrement = 0;
    md.hpGrowth = 1.010;
    md.armorInitial = 1.0;

    IngameCore core(level);

    core.init(pFbg);

    double lastFrame = glfwGetTime();
    double lastUpdate = lastFrame;
    int framesSinceLastUpdate = 0;
    double lastGameFrame = lastFrame;

    do
    {
        ++framesSinceLastUpdate;
        double curTime = glfwGetTime();
        double dt = curTime - lastFrame;
        lastFrame = curTime;
        if (curTime - lastUpdate > 1.0)
        {
            printf("%.04fms | %.04fFPS\n", (curTime - lastUpdate) / framesSinceLastUpdate,
                framesSinceLastUpdate / (curTime - lastUpdate));
            framesSinceLastUpdate = 0;
            lastUpdate = curTime;
        }

        int gameFrames = 0;
        if (core.inputHandler.getSpeedMultiplier() == 0)
        {
            // Don't keep counting frames if game is paused
            lastGameFrame = curTime;

            if (core.inputHandler.getShouldFrameAdvance())
                gameFrames = 1;
        }
        else
        {
            gameFrames = (curTime - lastGameFrame) * core.inputHandler.getSpeedMultiplier() *
                         g_game.ingameFramerate;
            if (gameFrames > 0)
                lastGameFrame = curTime;
        }

        core.map.buildingController.tickBuildings(core.map, gameFrames, core.waveController.getWaveStoneSpeed());
        core.map.projectileController.tickProjectiles(gameFrames);
        core.manaPool.checkMaximumMana();
        core.map.enemyController.tickMonsters(core.map, gameFrames);
        core.map.structureController.tickStructures(core.map, gameFrames);
        core.waveController.tick(gameFrames);

        fbg_glfwClear();

        fbg_clear(pFbg, 0);

        // Begin render frame
        core.renderer.render(core);

        core.inputHandler.handleMouseInput();
        core.inputHandler.handleKeyboardInput();

        // Finish render frame
        fbg_draw(pFbg);
        fbg_flip(pFbg);

    } while (bKeepRunning && !fbg_glfwShouldClose(pFbg));

    fbg_close(pFbg);

    return 0;
}
