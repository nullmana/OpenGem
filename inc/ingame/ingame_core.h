#pragma once

#include "constants/game_header.h"
#include "constants/status.h"

#include "ingame/ingame_input_handler.h"
#include "ingame/ingame_inventory.h"
#include "ingame/ingame_level_definition.h"
#include "ingame/ingame_map.h"
#include "ingame/ingame_renderer.h"

class IngameCore
{
private:
    struct GraphicsContext* pContext;

public:
    IngameCore(IngameLevelDefinition& level) : renderer(*this), map(level), inventory(36) {}

    STATUS init(struct GraphicsContext* context);

    struct GraphicsContext* context() { return pContext; }
    const struct GraphicsContext* context() const { return pContext; }

    IngameInputHandler inputHandler;
    IngameInventory inventory;
    IngameMap map;
    IngameRenderer renderer;
};
