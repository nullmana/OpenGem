#pragma once

#include "constants/game_header.h"
#include "constants/status.h"

#include "ingame/ingame_input_handler.h"
#include "ingame/ingame_level_definition.h"
#include "ingame/ingame_map.h"
#include "ingame/ingame_renderer.h"

class IngameCore
{
private:
	struct _fbg *pFbg;

public:
	IngameCore(IngameLevelDefinition &level) : renderer(*this), map(level) {}

	STATUS init(struct _fbg *pFbg_);

	struct _fbg * fbg() { return pFbg; }
	const struct _fbg * fbg()const { return pFbg; }

	IngameInputHandler inputHandler;
	IngameMap map;
	IngameRenderer renderer;
};
