#define DEFFN_BUTTON_BUILD_INPUT(fnname, targetState)                                        \
    static void fnname(Button& thisb, GLFWwindow* pWindow, int button, int action, int mods) \
    {                                                                                        \
        if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))                    \
        {                                                                                    \
            IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);              \
            pCore->inputHandler.toggleInputState(targetState);                               \
            if (pCore->inputHandler.getInputState() == INPUT_IDLE)                           \
                thisb.state &= ~BUTTON_ACTIVE;                                               \
            else                                                                             \
                thisb.state |= BUTTON_ACTIVE;                                                \
        }                                                                                    \
    }

DEFFN_BUTTON_BUILD_INPUT(buttonBuildWall_handleMouseInput, INPUT_BUILD_WALL);
DEFFN_BUTTON_BUILD_INPUT(buttonBuildTower_handleMouseInput, INPUT_BUILD_TOWER);
DEFFN_BUTTON_BUILD_INPUT(buttonBuildAmplifier_handleMouseInput, INPUT_BUILD_AMPLIFIER);
DEFFN_BUTTON_BUILD_INPUT(buttonBuildShrineCB_handleMouseInput, INPUT_BUILD_SHRINE_CB);
DEFFN_BUTTON_BUILD_INPUT(buttonBuildTrap_handleMouseInput, INPUT_BUILD_TRAP);
DEFFN_BUTTON_BUILD_INPUT(buttonBuildShrineLI_handleMouseInput, INPUT_BUILD_SHRINE_LI);
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

#define DEFFN_BUTTON_BUILD_HOVER(fnname)                                             \
    static void fnname(Button& thisb, GLFWwindow* pWindow, double xpos, double ypos) \
    {                                                                                \
        IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);          \
        thisb.state |= BUTTON_HOVER;                                                 \
    }

DEFFN_BUTTON_BUILD_HOVER(button_handleMouseHover);
#undef DEFFN_BUTTON_BUILD_HOVER

#define DEFFN_BUTTON_GEM_INPUT(fnname, gemType)                                              \
    static void fnname(Button& thisb, GLFWwindow* pWindow, int button, int action, int mods) \
    {                                                                                        \
        if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))                    \
        {                                                                                    \
            IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);              \
            pCore->inputHandler.startCreateGem(gemType);                                     \
            if (pCore->inputHandler.getCreatingGemType() == -1)                              \
                thisb.state &= ~BUTTON_ACTIVE;                                               \
            else                                                                             \
                thisb.state |= BUTTON_ACTIVE;                                                \
        }                                                                                    \
    }

DEFFN_BUTTON_GEM_INPUT(buttonGemSL_handleMouseInput, GEM_SLOW);
DEFFN_BUTTON_GEM_INPUT(buttonGemCH_handleMouseInput, GEM_CHAIN);
DEFFN_BUTTON_GEM_INPUT(buttonGemPO_handleMouseInput, GEM_POISON);
DEFFN_BUTTON_GEM_INPUT(buttonGemAT_handleMouseInput, GEM_ARMOR);
DEFFN_BUTTON_GEM_INPUT(buttonGemSH_handleMouseInput, GEM_SHOCK);
DEFFN_BUTTON_GEM_INPUT(buttonGemBB_handleMouseInput, GEM_BLOODBOUND);
DEFFN_BUTTON_GEM_INPUT(buttonGemCR_handleMouseInput, GEM_CRITICAL);
DEFFN_BUTTON_GEM_INPUT(buttonGemML_handleMouseInput, GEM_LEECH);
DEFFN_BUTTON_GEM_INPUT(buttonGemPB_handleMouseInput, GEM_POOLBOUND);
DEFFN_BUTTON_GEM_INPUT(buttonGemSP_handleMouseInput, GEM_SUPPRESSING);
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

#define DEFFN_BUTTON_SPEED_INPUT(fnname, targetSpeed, elseSpeed)                             \
    static void fnname(Button& thisb, GLFWwindow* pWindow, int button, int action, int mods) \
    {                                                                                        \
        if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))                    \
        {                                                                                    \
            IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);              \
            if (pCore->inputHandler.getSpeedMultiplier() != targetSpeed)                     \
                pCore->inputHandler.setSpeedMultiplier(targetSpeed);                         \
            else                                                                             \
                pCore->inputHandler.setSpeedMultiplier(elseSpeed);                           \
        }                                                                                    \
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
