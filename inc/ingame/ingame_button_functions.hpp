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
