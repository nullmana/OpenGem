#include "ingame/ingame_input_handler.h"
#include "ingame/ingame_core.h"

#include "wrapfbg.h"

#include <cstdio>

IngameInputHandler::IngameInputHandler(IngameCore& core_) : core(core_)
{
    speedMultiplier = 0;
    savedSpeedMultiplier = 1;
    pendingFrameAdvance = false;
    creatingGemType = -1;
}

static void mouseButtonCallback(GLFWwindow* pWindow, int button, int action, int mods)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);

    double xpos, ypos;
    glfwGetCursorPos(pWindow, &xpos, &ypos);

    Window* rootWindow = pCore->renderer.getRootWindow();
    if (rootWindow->contains(xpos, ypos))
        rootWindow->handleMouseInput(pWindow, button, action, mods);

    switch (pCore->inputHandler.getInputState())
    {
        case INPUT_DRAGGING_IDLE:
        case INPUT_DRAGGING_COMBINE:
        case INPUT_DRAGGING_BOMB:
            if ((action == GLFW_RELEASE) && (button == GLFW_MOUSE_BUTTON_LEFT))
            {
                pCore->inventory.clearDraggedGem();
                pCore->inputHandler.setInputState(INPUT_IDLE);
            }
            break;
    }
}

static void scrollCallback(GLFWwindow* pWindow, double xoffset, double yoffset) {}

static void keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
    IngameInputHandler* pInputHandler = &pCore->inputHandler;

    double xpos, ypos;
    glfwGetCursorPos(pWindow, &xpos, &ypos);

    Window* rootWindow = pCore->renderer.getRootWindow();
    if (rootWindow->contains(xpos, ypos))
        rootWindow->handleKeyInput(pWindow, key, scancode, action, mods);

    switch (key)
    {
        case GLFW_KEY_W:
            if (action == GLFW_PRESS)
                pInputHandler->toggleInputState(INPUT_BUILD_WALL);
            break;
        case GLFW_KEY_SPACE:
            if (action == GLFW_PRESS)
                pInputHandler->togglePause();
            break;
        case GLFW_KEY_Q:
            if (action == GLFW_PRESS)
                pInputHandler->cycleSpeedMultiplier();
            break;
        case GLFW_KEY_R:
            if (action == GLFW_PRESS)
                pInputHandler->toggleInputState(INPUT_BUILD_TRAP);
            break;
        case GLFW_KEY_T:
            if (action == GLFW_PRESS)
                pInputHandler->toggleInputState(INPUT_BUILD_TOWER);
            break;
        case GLFW_KEY_A:
            if (action == GLFW_PRESS)
                pInputHandler->toggleInputState(INPUT_BUILD_AMPLIFIER);
            break;
        case GLFW_KEY_G:
            if (action == GLFW_PRESS)
                pInputHandler->toggleInputState(INPUT_COMBINE_GEM);
            break;
        case GLFW_KEY_B:
            if (action == GLFW_PRESS)
            {
                Gem* pFirstGem = pCore->inventory.getFirstGem();
                if ((mods & GLFW_MOD_SHIFT) && (pFirstGem != NULL))
                    pInputHandler->toggleInputState(INPUT_BOMB_MULTIPLE);
                else
                    pInputHandler->toggleInputState(INPUT_BOMB_GEM);
            }
            break;
        case GLFW_KEY_M:
            if (g_game.game == GC_LABYRINTH)
            {
                if (action == GLFW_PRESS)
                    pCore->manaPool.castExpandManaPool();
            }
            break;
        case GLFW_KEY_KP_1:
            if (action == GLFW_PRESS)
                pInputHandler->startCreateGem(6);
            break;
        case GLFW_KEY_KP_2:
            if (action == GLFW_PRESS)
                pInputHandler->startCreateGem(7);
            break;
        case GLFW_KEY_KP_3:
            if (action == GLFW_PRESS)
                pInputHandler->startCreateGem(8);
            break;
        case GLFW_KEY_KP_4:
            if (action == GLFW_PRESS)
                pInputHandler->startCreateGem(3);
            break;
        case GLFW_KEY_KP_5:
            if (g_game.game != GC_LABYRINTH)
            {
                if (action == GLFW_PRESS)
                    pInputHandler->startCreateGem(4);
            }
            break;
        case GLFW_KEY_KP_6:
            if (action == GLFW_PRESS)
                pInputHandler->startCreateGem(5);
            break;
        case GLFW_KEY_KP_7:
            if (action == GLFW_PRESS)
                pInputHandler->startCreateGem(0);
            break;
        case GLFW_KEY_KP_8:
            if (action == GLFW_PRESS)
                pInputHandler->startCreateGem(1);
            break;
        case GLFW_KEY_KP_9:
            if (action == GLFW_PRESS)
                pInputHandler->startCreateGem(2);
            break;
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            switch (pInputHandler->getInputState())
            {
                case INPUT_BUILD_WALL_DRAGGING:
                case INPUT_DRAGGING_IDLE:
                case INPUT_DRAGGING_COMBINE:
                    break;
                case INPUT_BOMB_GEM:
                    if (action == GLFW_PRESS)
                    {
                        Gem* pFirstGem = pCore->inventory.getFirstGem();
                        if (pFirstGem != NULL)
                            pInputHandler->setInputState(INPUT_BOMB_TEMPLATE);
                    }
                    // Intentionally no break
                default:
                    if (action == GLFW_RELEASE)
                        pInputHandler->setInputState(INPUT_IDLE);
                    break;
            }
            break;
    }
}

STATUS IngameInputHandler::init()
{
    struct _fbg_glfw_context* pGlfwContext = (struct _fbg_glfw_context*)core.fbg()->user_context;

    glfwSetMouseButtonCallback(pGlfwContext->window, mouseButtonCallback);
    glfwSetScrollCallback(pGlfwContext->window, scrollCallback);
    glfwSetKeyCallback(pGlfwContext->window, keyCallback);

    glfwSetInputMode(pGlfwContext->window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    glfwSetInputMode(pGlfwContext->window, GLFW_STICKY_KEYS, GLFW_TRUE);

    inputState = INPUT_IDLE;

    setSpeedMultiplier(0);

    return STATUS_OK;
}

STATUS IngameInputHandler::handleMouseInput()
{
    struct _fbg_glfw_context* pGlfwContext = (struct _fbg_glfw_context*)core.fbg()->user_context;
    Gem* pDraggedGem = core.inventory.getDraggedGem();
    double xpos, ypos;

    glfwGetCursorPos(pGlfwContext->window, &xpos, &ypos);

    if (pDraggedGem != NULL)
    {
        pDraggedGem->x = xpos;
        pDraggedGem->y = ypos;
    }

    Window* rootWindow = core.renderer.getRootWindow();
    rootWindow->handleMouseHover(pGlfwContext->window, xpos, ypos);

    return STATUS_OK;
}

#ifdef DEBUG
#define BENCHMARK_MONSTERS 100E3
#endif

STATUS IngameInputHandler::handleKeyboardInput()
{
#ifdef DEBUG
    struct _fbg_glfw_context* pGlfwContext = (struct _fbg_glfw_context*)core.fbg()->user_context;

    if (glfwGetKey(pGlfwContext->window, GLFW_KEY_O) == GLFW_PRESS)
    {
        core.map.pathfinder.debugDrawPathWeights(core, -1);
    }
    for (int key = 0; key <= 9; ++key)
    {
        if (glfwGetKey(pGlfwContext->window, GLFW_KEY_0 + key) == GLFW_PRESS)
        {
            core.map.pathfinder.debugDrawPathWeights(core, key);
            break;
        }
    }
    if (glfwGetKey(pGlfwContext->window, GLFW_KEY_N) == GLFW_PRESS)
    {
        if (glfwGetKey(pGlfwContext->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            if (core.map.enemyController.getMonsters().size() < BENCHMARK_MONSTERS)
                core.map.enemyController.spawnMonsters(core.map.pathfinder,
                    BENCHMARK_MONSTERS - core.map.enemyController.getMonsters().size());
        }
        else
        {
            core.map.enemyController.spawnMonsters(core.map.pathfinder, 100);
        }
    }
    if (glfwGetKey(pGlfwContext->window, GLFW_KEY_M) == GLFW_PRESS)
    {
        if (glfwGetKey(pGlfwContext->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            core.manaPool.addMana(100, true);
        }
    }
#endif
    return STATUS_OK;
}

void IngameInputHandler::toggleInputState(INGAME_INPUT_STATE state)
{
    if (state == inputState)
    {
        setInputState(INPUT_IDLE);
    }
    else
    {
        switch (state)
        {
            case INPUT_COMBINE_GEM:
                if (inputState == INPUT_DRAGGING_COMBINE)
                    setInputState(INPUT_DRAGGING_IDLE);
                else if ((inputState == INPUT_DRAGGING_IDLE) || (inputState == INPUT_DRAGGING_BOMB))
                    setInputState(INPUT_DRAGGING_COMBINE);
                else
                    setInputState(INPUT_COMBINE_GEM);
                break;
            case INPUT_BOMB_GEM:
            case INPUT_BOMB_TEMPLATE:
            case INPUT_BOMB_MULTIPLE:
                if (inputState == INPUT_DRAGGING_BOMB)
                    setInputState(INPUT_DRAGGING_IDLE);
                else if ((inputState == INPUT_DRAGGING_IDLE) ||
                         (inputState == INPUT_DRAGGING_COMBINE))
                    setInputState(INPUT_DRAGGING_BOMB);
                else if ((inputState == INPUT_BOMB_GEM) || (inputState == INPUT_BOMB_MULTIPLE) ||
                         (inputState == INPUT_BOMB_TEMPLATE))
                    setInputState(INPUT_IDLE);
                else
                    setInputState(state);
                break;
            default:
                setInputState(state);
                break;
        }
    }
}

void IngameInputHandler::setInputState(INGAME_INPUT_STATE state)
{
    switch (inputState)
    {
        case INPUT_BUILD_WALL:
            core.renderer.setBuildButtonActive(0, false);
            break;
        case INPUT_BUILD_TOWER:
            core.renderer.setBuildButtonActive(1, false);
            break;
        case INPUT_BUILD_AMPLIFIER:
            core.renderer.setBuildButtonActive(2, false);
            break;
        case INPUT_BUILD_TRAP:
            core.renderer.setBuildButtonActive(4, false);
            break;
        case INPUT_CREATE_GEM:
            if (creatingGemType != -1)
            {
                core.renderer.setGemButtonActive(creatingGemType, false);
                creatingGemType = -1;
            }
            break;
        case INPUT_DRAGGING_COMBINE:
            if (state != INPUT_DRAGGING_IDLE)
                core.inventory.clearDraggedGem();
            // Intentionally no break
        case INPUT_COMBINE_GEM:
        {
            int button = g_game.game == GC_LABYRINTH ? 7 : 5;
            core.renderer.setBuildButtonActive(button, false);
            break;
        }
        case INPUT_DRAGGING_BOMB:
            if (state != INPUT_DRAGGING_IDLE)
                core.inventory.clearDraggedGem();
            // Intentionally no break
        case INPUT_BOMB_MULTIPLE:
        case INPUT_BOMB_TEMPLATE:
        case INPUT_BOMB_GEM:
        {
            int button = g_game.game == GC_LABYRINTH ? 6 : 3;
            core.renderer.setBuildButtonForceColor(button, -1);
            core.renderer.setBuildButtonActive(button, false);
            break;
        }
        case INPUT_DRAGGING_IDLE:
            if ((state != INPUT_DRAGGING_COMBINE) && (state != INPUT_DRAGGING_BOMB))
                core.inventory.clearDraggedGem();
            break;
    }

    inputState = state;

    switch (state)
    {
        case INPUT_BUILD_WALL:
            core.renderer.setBuildButtonActive(0, true);
            break;
        case INPUT_BUILD_TOWER:
            core.renderer.setBuildButtonActive(1, true);
            break;
        case INPUT_BUILD_AMPLIFIER:
            core.renderer.setBuildButtonActive(2, true);
            break;
        case INPUT_BUILD_TRAP:
            core.renderer.setBuildButtonActive(4, true);
            break;
        case INPUT_COMBINE_GEM:
        case INPUT_DRAGGING_COMBINE:
        {
            int button = g_game.game == GC_LABYRINTH ? 7 : 5;
            core.renderer.setBuildButtonActive(button, true);
            break;
        }
        case INPUT_BOMB_GEM:
        case INPUT_BOMB_TEMPLATE:
        case INPUT_BOMB_MULTIPLE:
        case INPUT_DRAGGING_BOMB:
        {
            int button = g_game.game == GC_LABYRINTH ? 6 : 3;
            if (state == INPUT_BOMB_TEMPLATE)
                core.renderer.setBuildButtonForceColor(button, 8);
            if (state == INPUT_BOMB_MULTIPLE)
                core.renderer.setBuildButtonForceColor(button, 9);
            core.renderer.setBuildButtonActive(button, true);
            break;
        }
    }

#ifdef DEBUG
    printf("Input state = %s\n", INPUT_STATE_NAME[inputState]);
#endif
}

void IngameInputHandler::startCreateGem(int gemType)
{
    if (gemType == creatingGemType)
    {
        setInputState(INPUT_IDLE);
        creatingGemType = -1;
    }
    else
    {
        setInputState(INPUT_CREATE_GEM);
        creatingGemType = gemType;
        core.renderer.setGemButtonActive(gemType, true);
    }
}

void IngameInputHandler::togglePause()
{
    if (speedMultiplier == 0)
    {
        setSpeedMultiplier(savedSpeedMultiplier);
    }
    else
    {
        savedSpeedMultiplier = speedMultiplier;
        setSpeedMultiplier(0);
    }
}

void IngameInputHandler::cycleSpeedMultiplier()
{
    if (speedMultiplier == 0)
        setSpeedMultiplier(savedSpeedMultiplier);
    else if (speedMultiplier == 9)
        setSpeedMultiplier(1);
    else
        setSpeedMultiplier(speedMultiplier * 3);
}

static int getSpeedButton(int speed)
{
    if (speed == 3)
        return 2;
    else if (speed == 9)
        return 3;
    else
        return speed;
}
void IngameInputHandler::setSpeedMultiplier(int speed)
{
    core.renderer.setSpeedButtonActive(getSpeedButton(speedMultiplier), false);
    speedMultiplier = speed;
    core.renderer.setSpeedButtonActive(getSpeedButton(speed), true);
#ifdef DEBUG
    printf("Speed = %d\n", speedMultiplier);
#endif
}
