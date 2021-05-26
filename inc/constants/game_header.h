#pragma once

enum GC_GAME
{
    GC_LABYRINTH = 2,
    GC_CHASINGSHADOWS = 3,
    GC_FROSTBORNWRATH = 4,
};

#define GCL_INGAME_MAP_WIDTH 20
#define GCL_INGAME_MAP_HEIGHT 17
#define GCL_INGAME_BUILDING_SIZE 1
#define GCL_INGAME_FRAMERATE 30

#define GCCS_INGAME_MAP_WIDTH 54
#define GCCS_INGAME_MAP_HEIGHT 32
#define GCCS_INGAME_BUILDING_SIZE 2
#define GCCS_INGAME_FRAMERATE 30

struct GameHeader
{
    GC_GAME game;

    int ingameMapWidth;
    int ingameMapHeight;
    int ingameBuildingSize;
    int ingameFramerate;
};

extern GameHeader g_game;
