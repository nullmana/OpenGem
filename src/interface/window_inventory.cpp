#include "interface/window_inventory.h"

#include "ingame/ingame_core.h"
#include "ingame/ingame_inventory.h"

#include "glfw/fbg_glfw.h"

STATUS WindowInventory::render(struct _fbg *pFbg)
{
	fbg_rect(pFbg, x-2, y-2, width+4, height+4, 0x1D, 0x22, 0x2E);

	return inventory.render(pFbg, *this);
}

void WindowInventory::handleMouseInput(GLFWwindow *pWindow, int button, int action, int mods)
{
	IngameCore *pCore = (IngameCore *)glfwGetWindowUserPointer(pWindow);
	IngameInputHandler *pInputHandler = &pCore->inputHandler;
	double xpos, ypos;
	float scale = width / 3.0f;

	glfwGetCursorPos(pWindow, &xpos, &ypos);
}
