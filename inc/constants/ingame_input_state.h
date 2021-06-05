#pragma once

enum INGAME_INPUT_STATE
{
    INPUT_IDLE = 0x00,

    INPUT_BUILD_WALL,
    INPUT_BUILD_WALL_DRAGGING,
    INPUT_BUILD_TOWER,
    INPUT_BUILD_TRAP,
    INPUT_BUILD_AMPLIFIER,
    INPUT_BUILD_SHRINE_CB,
    INPUT_BUILD_SHRINE_LI,

    INPUT_CREATE_GEM,
    INPUT_COMBINE_GEM,
    INPUT_BOMB_GEM,

    INPUT_DRAGGING_IDLE,
    INPUT_DRAGGING_COMBINE,
    INPUT_DRAGGING_BOMB,

    INPUT_BOMB_MULTIPLE,
    INPUT_BOMB_TEMPLATE,

    INGAME_INPUT_STATE_COUNT
};

#ifdef DEBUG
static const char* INPUT_STATE_NAME[INGAME_INPUT_STATE_COUNT] = {
    "None",

    "BuildWall",
    "BuildWallDragging",
    "BuildTower",
    "BuildTrap",
    "BuildAmplifier",
    "BuildShrineCB",
    "BuildShrineLI",

    "CreateGem",
    "CombineGem",
    "BombGem",

    "DraggingIdle",
    "DraggingCombine",
    "DraggingBomb",

    "BombMultiple",
    "BombTemplate",
};
#endif
