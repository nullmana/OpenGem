#define DEFFN_BUTTON_BUILD_INPUT(fnname, targetState)                                              \
    static void fnname(Button& thisb, GLFWwindow* pWindow, int button, int action, int mods)       \
    {                                                                                              \
        if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))                          \
        {                                                                                          \
            IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);                    \
            pCore->inputHandler.toggleInputState(targetState);                                     \
            if (pCore->inputHandler.getInputState() == INPUT_IDLE)                                 \
                thisb.state &= ~BUTTON_ACTIVE;                                                     \
            else                                                                                   \
                thisb.state |= BUTTON_ACTIVE;                                                      \
        }                                                                                          \
    }

DEFFN_BUTTON_BUILD_INPUT(buttonBuildWall_handleMouseInput, INPUT_BUILD_WALL);
DEFFN_BUTTON_BUILD_INPUT(buttonBuildTower_handleMouseInput, INPUT_BUILD_TOWER);
DEFFN_BUTTON_BUILD_INPUT(buttonBuildAmplifier_handleMouseInput, INPUT_BUILD_AMPLIFIER);
// Charged Bolt Shrine (C)
DEFFN_BUTTON_BUILD_INPUT(buttonBuildTrap_handleMouseInput, INPUT_BUILD_TRAP);
// Lightning Shrine (L)
// Gem Bomb (B)
DEFFN_BUTTON_BUILD_INPUT(buttonBuildCombine_handleMouseInput, INPUT_COMBINE_GEM);
#undef DEFFN_BUTTON_BUILD_INPUT

static void buttonBuildBomb_handleMouseInput(
    Button& thisb, GLFWwindow* pWindow, int button, int action, int mods)
{
    if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
    {
        IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
        Gem* pFirstGem = pCore->inventory.getFirstGem();
        if ((mods & GLFW_MOD_SHIFT) && (pFirstGem != NULL))
            pCore->inputHandler.toggleInputState(INPUT_BOMB_MULTIPLE);
        else
            pCore->inputHandler.toggleInputState(INPUT_BOMB_GEM);

        switch (pCore->inputHandler.getInputState())
        {
            case INPUT_BOMB_MULTIPLE:
                thisb.forceColor = 9;
                break;
            case INPUT_BOMB_GEM:
                thisb.forceColor = -1;
                thisb.state |= BUTTON_ACTIVE;
                break;
            default:
                thisb.forceColor = -1;
                thisb.state &= ~BUTTON_ACTIVE;
                break;
        }
    }
}
static void buttonBuildMana_handleMouseInput(
    Button& thisb, GLFWwindow* pWindow, int button, int action, int mods)
{
    if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
    {
        IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
        if (mods & GLFW_MOD_SHIFT)
        {
            if (pCore->manaPool.toggleAutopool())
                thisb.state |= BUTTON_ACTIVE;
            else
                thisb.state &= ~BUTTON_ACTIVE;
        }
        else
        {
            pCore->manaPool.castExpandManaPool();
        }
    }
}

#define DEFFN_BUTTON_BUILD_HOVER(fnname)                                                           \
    static void fnname(Button& thisb, GLFWwindow* pWindow, double xpos, double ypos)               \
    {                                                                                              \
        IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);                        \
        thisb.state |= BUTTON_HOVER;                                                               \
    }

DEFFN_BUTTON_BUILD_HOVER(button_handleMouseHover);
#undef DEFFN_BUTTON_BUILD_HOVER

#define DEFFN_BUTTON_GEM_INPUT(fnname, gemType)                                                    \
    static void fnname(Button& thisb, GLFWwindow* pWindow, int button, int action, int mods)       \
    {                                                                                              \
        if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))                          \
        {                                                                                          \
            IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);                    \
            pCore->inputHandler.startCreateGem(gemType);                                           \
            if (pCore->inputHandler.getCreatingGemType() == -1)                                    \
                thisb.state &= ~BUTTON_ACTIVE;                                                     \
            else                                                                                   \
                thisb.state |= BUTTON_ACTIVE;                                                      \
        }                                                                                          \
    }

DEFFN_BUTTON_GEM_INPUT(buttonGem0_handleMouseInput, 0);
DEFFN_BUTTON_GEM_INPUT(buttonGem1_handleMouseInput, 1);
DEFFN_BUTTON_GEM_INPUT(buttonGem2_handleMouseInput, 2);
DEFFN_BUTTON_GEM_INPUT(buttonGem3_handleMouseInput, 3);
DEFFN_BUTTON_GEM_INPUT(buttonGem4_handleMouseInput, 4);
DEFFN_BUTTON_GEM_INPUT(buttonGem5_handleMouseInput, 5);
DEFFN_BUTTON_GEM_INPUT(buttonGem6_handleMouseInput, 6);
DEFFN_BUTTON_GEM_INPUT(buttonGem7_handleMouseInput, 7);
DEFFN_BUTTON_GEM_INPUT(buttonGem8_handleMouseInput, 8);
#undef DEFFN_BUTTON_GEM_INPUT

static void buttonGemAnvil_handleMouseInput(
    Button& thisb, GLFWwindow* pWindow, int button, int action, int mods)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);

    if (button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (action == GLFW_RELEASE)
    {
        Gem* pDraggedGem = pCore->inventory.getDraggedGem();
        if (pDraggedGem != NULL)
        {
            if (pCore->inputHandler.getInputState() == INPUT_DRAGGING_BOMB)
            {
                pCore->inventory.salvageGem(pDraggedGem);
            }
            else if ((pCore->inputHandler.getInputState() == INPUT_DRAGGING_IDLE) ||
                     (pCore->inputHandler.getInputState() == INPUT_DRAGGING_COMBINE))
            {
                if (pCore->manaPool.getMana() >= pDraggedGem->manaCost)
                {
                    if (NULL != pCore->inventory.duplicateGemIntoSlot(pDraggedGem, -1))
                        pCore->manaPool.addMana(-pDraggedGem->manaCost, false);
                }
            }
            pCore->inputHandler.setInputState(INPUT_IDLE);
        }
    }
    else if (action == GLFW_PRESS)
    {
        Gem* pFirstGem = pCore->inventory.getFirstGem();
        bool bClearState = true;
        if (pFirstGem != NULL)
        {
            if (pCore->inputHandler.getInputState() == INPUT_BOMB_MULTIPLE)
            {
                pCore->inventory.salvageGem(pFirstGem);
                if (pCore->inventory.getFirstGem() != NULL)
                    bClearState = false;
            }
        }
        if (bClearState)
            pCore->inputHandler.setInputState(INPUT_IDLE);
    }
}

#define DEFFN_BUTTON_SPEED_INPUT(fnname, targetSpeed, elseSpeed)                                   \
    static void fnname(Button& thisb, GLFWwindow* pWindow, int button, int action, int mods)       \
    {                                                                                              \
        if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))                          \
        {                                                                                          \
            IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);                    \
            if (pCore->inputHandler.getSpeedMultiplier() != targetSpeed)                           \
                pCore->inputHandler.setSpeedMultiplier(targetSpeed);                               \
            else                                                                                   \
                pCore->inputHandler.setSpeedMultiplier(elseSpeed);                                 \
        }                                                                                          \
    }

DEFFN_BUTTON_SPEED_INPUT(buttonSpeed1_handleMouseInput, 1, 3);
DEFFN_BUTTON_SPEED_INPUT(buttonSpeed3_handleMouseInput, 3, 1);
DEFFN_BUTTON_SPEED_INPUT(buttonSpeed9_handleMouseInput, 9, 3);
static void buttonSpeed0_handleMouseInput(
    Button& thisb, GLFWwindow* pWindow, int button, int action, int mods)
{
    if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
    {
        IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
        if (pCore->inputHandler.getSpeedMultiplier() == 0)
            pCore->inputHandler.setShouldFrameAdvance();
        else
            pCore->inputHandler.setSpeedMultiplier(0);
    }
}
#undef DEFFN_BUTTON_SPEED_INPUT
