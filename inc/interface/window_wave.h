#pragma once

#include "interface/window.h"

class IngameWaveController;

class WindowWave : public Window
{
private:
    IngameWaveController& waveController;

    int getWaveStone(float ypos) const;

public:
    WindowWave(IngameWaveController& waveController_)
        : waveController(waveController_) {}

    virtual STATUS render(struct _fbg* pFbg);

    virtual void handleMouseInput(GLFWwindow* pWindow, int button, int action, int mods);
    virtual void handleKeyInput(GLFWwindow* pWindow, int key, int scancode, int action, int mods);
};
