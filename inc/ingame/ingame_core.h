#pragma once

#include "constants/game_header.h"
#include "constants/status.h"

#include "ingame/ingame_input_handler.h"
#include "ingame/ingame_inventory.h"
#include "ingame/ingame_level_definition.h"
#include "ingame/ingame_mana_pool.h"
#include "ingame/ingame_map.h"
#include "ingame/ingame_renderer.h"
#include "ingame/ingame_wave_controller.h"

class IngameCore
{
private:
    struct _fbg* pFbg;

public:
    IngameCore(IngameLevelDefinition& level);

    STATUS init(struct _fbg* pFbg_);

    struct _fbg* fbg() { return pFbg; }
    const struct _fbg* fbg() const { return pFbg; }

    IngameInputHandler inputHandler;
    IngameInventory inventory;
    IngameManaPool manaPool;
    IngameMap map;
    IngameRenderer renderer;
    IngameWaveController waveController;
};
