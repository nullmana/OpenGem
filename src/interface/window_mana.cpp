#include "interface/window_mana.h"

#include "ingame/ingame_core.h"
#include "ingame/ingame_mana_pool.h"

#include "wrapfbg.h"

STATUS WindowMana::render(struct _fbg* pFbg)
{
    fbg_rect(pFbg, x - 2, y - 2, width + 4, height + 4, 0x1D, 0x22, 0x2E);

    return manaPool.render(pFbg, *this);
}

void WindowMana::handleMouseInput(GLFWwindow* pWindow, int button, int action, int mods)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);

    Gem* pDraggedGem = pCore->inventory.getDraggedGem();

    if ((pDraggedGem != NULL) && (action == GLFW_RELEASE) && (button == GLFW_MOUSE_BUTTON_LEFT))
    {
        pCore->inventory.salvageGem(pDraggedGem);
    }
}
