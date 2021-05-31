#pragma once

#include "constants/ingame_input_state.h"
#include "constants/status.h"

class IngameCore;

class IngameInputHandler
{
private:
    IngameCore& core; // InputHandler needs to break encapsulation, try to limit use
    INGAME_INPUT_STATE inputState;
    int speedMultiplier;
    int savedSpeedMultiplier;

    int creatingGemType;

    bool pendingFrameAdvance;

public:
    IngameInputHandler(IngameCore& core_);
    STATUS init();

    STATUS handleMouseInput();
    STATUS handleKeyboardInput();

    void toggleInputState(INGAME_INPUT_STATE state);
    void setInputState(INGAME_INPUT_STATE state);
    INGAME_INPUT_STATE getInputState() { return inputState; }
    void startCreateGem(int gemType);
    int getCreatingGemType() { return creatingGemType; }

    void togglePause();
    void cycleSpeedMultiplier();
    void setSpeedMultiplier(int speed);
    int getSpeedMultiplier() { return speedMultiplier; }

    void setShouldFrameAdvance() { pendingFrameAdvance = true; }
    bool getShouldFrameAdvance()
    {
        bool ret = pendingFrameAdvance;
        pendingFrameAdvance = false;
        return ret;
    }
};
