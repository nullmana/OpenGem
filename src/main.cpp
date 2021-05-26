#include "glfw/fbg_glfw.h"

#include "ingame/ingame_core.h"

#include <cstdio>
#include <cstring>

bool bKeepRunning = true;

GameHeader g_game;

int main(int argc, char * argv[]){
	if ((argc >= 2) && !strcmp(argv[1], "cs"))
	{
		g_game.game = GC_CHASINGSHADOWS;
		g_game.ingameMapWidth = GCCS_INGAME_MAP_WIDTH;
		g_game.ingameMapHeight = GCCS_INGAME_MAP_HEIGHT;
		g_game.ingameBuildingSize = GCCS_INGAME_BUILDING_SIZE;
		g_game.ingameFramerate = GCCS_INGAME_FRAMERATE;
	}
	else
	{
		g_game.game = GC_LABYRINTH;
		g_game.ingameMapWidth = GCL_INGAME_MAP_WIDTH;
		g_game.ingameMapHeight = GCL_INGAME_MAP_HEIGHT;
		g_game.ingameBuildingSize = GCL_INGAME_BUILDING_SIZE;
		g_game.ingameFramerate = GCL_INGAME_FRAMERATE;
	}

	switch (g_game.game)
	{
		case GC_LABYRINTH:
			printf ("Selected Game: Labyrinth\n");
			break;
		case GC_CHASINGSHADOWS:
			printf ("Selected Game: Chasing Shadows\n");
			break;
		default:
			printf ("Error: Unknown Game: %d\n", g_game.game);
			return -1;
	}

	struct _fbg *pFbg = fbg_glfwSetup(1280, 720, 4, "opengem", 0, 0);
	if (pFbg == NULL)
		return -1;

	// Initialize level
	IngameLevelDefinition level;

	level.orbX = g_game.ingameMapWidth/2; level.orbY = g_game.ingameMapHeight/2;
	for (int y = g_game.ingameMapHeight/4; y < g_game.ingameMapHeight*3/4; ++y)
		for (int x = 0; x < g_game.ingameMapWidth; ++x)
			level.buildings.at(y, x) = BUILDING_PATH;
	for (int y = 0; y < g_game.ingameMapHeight/4; ++y)
		for (int x = g_game.ingameMapWidth*3/8; x < g_game.ingameMapWidth*5/8; ++x)
			level.buildings.at(y, x) = BUILDING_PATH;
	for (int y = g_game.ingameMapHeight*3/4; y < g_game.ingameMapHeight; ++y)
		for (int x = g_game.ingameMapWidth*3/8; x < g_game.ingameMapWidth*5/8; ++x)
			level.buildings.at(y, x) = BUILDING_PATH;

	level.buildings.at(level.orbY, level.orbX) = BUILDING_ORB;

	IngameCore core(level);

	core.init(pFbg);

	double lastFrame = glfwGetTime();
	double lastUpdate = lastFrame;
	int framesSinceLastUpdate = 0;
	double lastGameFrame = lastFrame;

	do {
		++framesSinceLastUpdate;
		double curTime = glfwGetTime();
		double dt = curTime - lastFrame;
		lastFrame = curTime;
		if (curTime - lastUpdate > 1.0)
		{
			printf("%.04fms | %.04fFPS\n", (curTime-lastUpdate) / framesSinceLastUpdate, framesSinceLastUpdate / (curTime-lastUpdate));
			framesSinceLastUpdate = 0;
			lastUpdate = curTime;
		}

		int gameFrames = 0;
		if (core.inputHandler.getSpeedMultiplier() == 0)
		{
			// Don't keep counting frames if game is paused
			lastGameFrame = curTime;
		}
		else
		{
			gameFrames = (curTime - lastGameFrame) * core.inputHandler.getSpeedMultiplier() * g_game.ingameFramerate;
			if (gameFrames > 0)
				lastGameFrame = curTime;
		}

		core.map.buildingController.tickBuildings(core.map, gameFrames);
		core.map.projectileController.tickProjectiles(gameFrames);
		core.map.enemyController.tickMonsters(core.map, gameFrames);

		fbg_glfwClear();

		fbg_clear(pFbg, 0);

		// Begin render frame
		core.renderer.render(core);

		core.inputHandler.handleMouseInput(core);
		core.inputHandler.handleKeyboardInput(core);

		// Finish render frame
		fbg_draw(pFbg);
		fbg_flip(pFbg);

	} while (bKeepRunning && !fbg_glfwShouldClose(pFbg));

	fbg_close(pFbg);

	return 0;
}
