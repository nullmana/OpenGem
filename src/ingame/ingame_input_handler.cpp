#include "ingame/ingame_input_handler.h"
#include "ingame/ingame_core.h"

#include "wrapfbg.h"

#include <cstdio>

IngameInputHandler::IngameInputHandler()
{
    speedMultiplier = 0;
    savedSpeedMultiplier = 1;
}

static void mouseButtonCallback(GLFWwindow* pWindow, int button, int action, int mods)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);

    double xpos, ypos;
    glfwGetCursorPos(pWindow, &xpos, &ypos);

    Window* rootWindow = pCore->renderer.getRootWindow();
    if (rootWindow->contains(xpos, ypos))
        rootWindow->handleMouseInput(pWindow, button, action, mods);
}

static void scrollCallback(GLFWwindow* pWindow, double xoffset, double yoffset) {}

static void keyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
{
    IngameCore* pCore = (IngameCore*)glfwGetWindowUserPointer(pWindow);
    IngameInputHandler* pInputHandler = &pCore->inputHandler;

    switch (key)
    {
        case GLFW_KEY_W:
            if (action == GLFW_PRESS)
            {
                pInputHandler->toggleInputState(INPUT_BUILD_WALL);
            }
            break;
        case GLFW_KEY_SPACE:
            if (action == GLFW_PRESS)
            {
                pInputHandler->togglePause();
            }
            break;
        case GLFW_KEY_Q:
            if (action == GLFW_PRESS)
            {
                pInputHandler->cycleSpeedMultiplier();
            }
            break;
        case GLFW_KEY_R:
            if (action == GLFW_PRESS)
            {
                pInputHandler->toggleInputState(INPUT_BUILD_TRAP);
            }
            break;
        case GLFW_KEY_T:
            if (action == GLFW_PRESS)
            {
                pInputHandler->toggleInputState(INPUT_BUILD_TOWER);
            }
            break;
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            if (action == GLFW_RELEASE)
            {
                INGAME_INPUT_STATE state = pInputHandler->getInputState();
                if (state != INPUT_BUILD_WALL_DRAGGING)
                    pInputHandler->setInputState(INPUT_IDLE);
            }
            break;
    }
}

STATUS IngameInputHandler::init(IngameCore& core)
{
    struct _fbg_glfw_context* pGlfwContext = (struct _fbg_glfw_context*)core.fbg()->user_context;

    glfwSetMouseButtonCallback(pGlfwContext->window, mouseButtonCallback);
    glfwSetScrollCallback(pGlfwContext->window, scrollCallback);
    glfwSetKeyCallback(pGlfwContext->window, keyCallback);

    glfwSetInputMode(pGlfwContext->window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    glfwSetInputMode(pGlfwContext->window, GLFW_STICKY_KEYS, GLFW_TRUE);

    inputState = INPUT_IDLE;

    return STATUS_OK;
}

STATUS IngameInputHandler::handleMouseInput(IngameCore& core)
{
    return STATUS_OK;
}

#ifdef DEBUG
#define BENCHMARK_MONSTERS 100E3
#endif

STATUS IngameInputHandler::handleKeyboardInput(IngameCore& core)
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
    if (glfwGetKey(pGlfwContext->window, GLFW_KEY_M) == GLFW_PRESS)
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
#endif
    return STATUS_OK;
}

void IngameInputHandler::toggleInputState(INGAME_INPUT_STATE state)
{
    if (state == inputState)
        inputState = INPUT_IDLE;
    else
        inputState = state;

#ifdef DEBUG
    printf("Input state = %s\n", INPUT_STATE_NAME[inputState]);
#endif
}

void IngameInputHandler::setInputState(INGAME_INPUT_STATE state)
{
    inputState = state;

#ifdef DEBUG
    printf("Input state = %s\n", INPUT_STATE_NAME[inputState]);
#endif
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

void IngameInputHandler::setSpeedMultiplier(int speed)
{
    speedMultiplier = speed;
#ifdef DEBUG
    printf("Speed = %d\n", speedMultiplier);
#endif
}
