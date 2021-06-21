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
    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
    IngameInputHandler* pInputHandler = &pCore->inputHandler;
    double xpos, ypos;

    glfwGetCursorPos(pWindow, &xpos, &ypos);

    int waveNum = getWaveStone(ypos);
    if (waveNum == -1)
        return;

    INGAME_INPUT_STATE state = pInputHandler->getInputState();
    if (action == GLFW_PRESS)
    {
        switch (state)
        {
            case INPUT_BOMB_MULTIPLE:
            case INPUT_BOMB_TEMPLATE:
            {
                Gem* pBombGem = pCore->inventory.getFirstGem();
                bool bClearState = true;
                if (pBombGem != NULL)
                {
                    if ((state == INPUT_BOMB_MULTIPLE) ||
                        (pCore->manaPool.getMana() >= pBombGem->manaCost))
                    {
                        waveController.enrageWave(waveNum, pBombGem->grade, 1);

                        if (state == INPUT_BOMB_TEMPLATE)
                        {
                            pCore->manaPool.addMana(-pBombGem->manaCost, false);
                            bClearState = false;
                        }
                        else
                        {
                            pCore->inventory.deleteGem(pBombGem);
                            if (pCore->inventory.getFirstGem() != NULL)
                                bClearState = false;
                        }
                    }
                }
                if (bClearState)
                    pInputHandler->setInputState(INPUT_IDLE);
                break;
            }
            default:
                if (!!(mods & GLFW_MOD_CONTROL))
                    waveController.callWavesEarly(INT_MAX);
                else
                    waveController.callWavesEarly(waveNum);
                break;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        if (state == INPUT_DRAGGING_BOMB)
        {
            Gem* pDraggedGem = pCore->inventory.getDraggedGem();
            if (pDraggedGem != NULL)
            {
                waveController.enrageWave(waveNum, pDraggedGem->grade, 1);

                pCore->inventory.deleteGem(pDraggedGem);
            }
            pInputHandler->setInputState(INPUT_IDLE);
        }
    }
}

void WindowWave::handleKeyInput(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
    double xpos, ypos;
    float scale = width / g_game.ingameMapWidth;

    glfwGetCursorPos(pWindow, &xpos, &ypos);

    int waveNum = getWaveStone(ypos);
    if (waveNum == -1)
        return;

    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_DELETE:
                waveController.clearEnrageWave(waveNum);
                break;
        }
    }
}
