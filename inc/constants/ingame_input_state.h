#pragma once

enum INGAME_INPUT_STATE
{
    INPUT_IDLE = 0x00,

    INPUT_BUILD_WALL,
    INPUT_BUILD_WALL_DRAGGING,
    INPUT_BUILD_TOWER,
    INPUT_BUILD_TRAP,

    INPUT_DRAGGING_IDLE,

    INGAME_INPUT_STATE_COUNT
};

#ifdef DEBUG
static const char* INPUT_STATE_NAME[INGAME_INPUT_STATE_COUNT] = {
    "None",

    "BuildWall",
    "BuildWallDragging",
    "BuildTower",
    "BuildTrap",

    "DraggingIdle",
};
#endif
