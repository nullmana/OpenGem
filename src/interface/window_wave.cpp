#include "interface/window_wave.h"

#include "ingame/ingame_core.h"
#include "ingame/ingame_wave_controller.h"

#include "constants/game_header.h"

#include "wrapfbg.h"

STATUS WindowWave::render(struct _fbg* pFbg)
{
    fbg_rect(pFbg, x - 2, y - 2, width + 4, height + 4, 0x1D, 0x22, 0x2E);

    return waveController.render(pFbg, *this);
}

int WindowWave::getWaveStone(float ypos) const
{
    return waveController.getWaveStoneAtPosition(ypos - y, height);
}

void WindowWave::handleMouseInput(GLFWwindow* pWindow, int button, int action, int mods)
{
    // TODO enraging

    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
    IngameInputHandler* pInputHandler = &pCore->inputHandler;
    double xpos, ypos;

    glfwGetCursorPos(pWindow, &xpos, &ypos);

    int waveNum = getWaveStone(ypos);

    if (waveNum != -1)
    {
        if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
        {
            if (!!(mods & GLFW_MOD_CONTROL))
                waveController.callWavesEarly(INT_MAX);
            else
                waveController.callWavesEarly(waveNum);
        }
    }
}
