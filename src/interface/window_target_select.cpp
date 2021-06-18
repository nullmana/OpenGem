#include "interface/window_target_select.h"

#include "ingame/ingame_core.h"

#include "entity/gem.h"

#include "wrapfbg.h"

WindowTargetSelect::WindowTargetSelect()
{
    pTargetGem = NULL;
    lastTargetOption = 0;
}

int WindowTargetSelect::cursorTargetOption(double xpos, double ypos) const
{
    // Add a small deadzone to the center
    if (abs(xpos - pTargetGem->x) + abs(ypos - pTargetGem->y) < 4.0)
        return lastTargetOption;

    double angle = atan2(ypos - pTargetGem->y, xpos - pTargetGem->x) * (180.0 / M_PI) + 112.5;
    return int((angle + 360.0) / 45.0) % 8;
}

STATUS WindowTargetSelect::render(struct _fbg* pFbg)
{
    static const int dx[9] = {-8, 8, 20, 20, 8, -8, -20, -20, -8};
    static const int dy[9] = {-20, -20, -8, 8, 20, 20, 8, -8, -20};

    if ((width != 0) && (pTargetGem != NULL))
    {
        int x = pTargetGem->x;
        int y = pTargetGem->y;

        for (int i = 0; i < 8; ++i)
        {
            uint8_t r = (i == lastTargetOption) ? 0xCC : 0x33;
            fbgx_tri(pFbg, x, y, x + dx[i], y + dy[i], x + dx[i + 1], y + dy[i + 1], r, r, r);
        }
    }
    return STATUS_OK;
}

void WindowTargetSelect::handleMouseInput(GLFWwindow* pWindow, int button, int action, int mods)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
    double xpos, ypos;

    if (pTargetGem == NULL)
        return;

    glfwGetCursorPos(pWindow, &xpos, &ypos);

    if ((action == GLFW_RELEASE) && ((button == GLFW_MOUSE_BUTTON_LEFT) || (button == GLFW_MOUSE_BUTTON_RIGHT)))
    {
        pTargetGem->targetPriority = TARGET_PRIORITY_TYPE(cursorTargetOption(xpos, ypos));

        pTargetGem = NULL;
        resize(0, 0, 0, 0);
    }
}

void WindowTargetSelect::handleMouseHover(GLFWwindow* pWindow, double xpos, double ypos)
{
    if (pTargetGem == NULL)
        return;

    lastTargetOption = cursorTargetOption(xpos, ypos);
}

void WindowTargetSelect::openTargetSelect(Gem* pGem)
{
    pTargetGem = pGem;

    lastTargetOption = (pGem->targetPriority + 9) % 8;

    resize(pParent->x, pParent->y, pParent->width, pParent->height);
}
