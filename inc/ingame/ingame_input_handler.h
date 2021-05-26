#pragma once

#include "constants/status.h"
#include "constants/ingame_input_state.h"

class IngameCore;

class IngameInputHandler
{
private:
	INGAME_INPUT_STATE inputState;
	int speedMultiplier;
	int savedSpeedMultiplier;
public:
	IngameInputHandler();
	STATUS init(IngameCore &core);

	STATUS handleMouseInput (IngameCore &core);
	STATUS handleKeyboardInput (IngameCore &core);

	void toggleInputState(INGAME_INPUT_STATE state);
	void setInputState(INGAME_INPUT_STATE state);
	INGAME_INPUT_STATE getInputState() { return inputState; }

	void togglePause();
	void cycleSpeedMultiplier();
	void setSpeedMultiplier(int speed);
	int getSpeedMultiplier() { return speedMultiplier; }

};
