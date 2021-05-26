#include "interface/window.h"

#include "glfw/fbg_glfw.h"

STATUS Window::render(struct _fbg *pFbg)
{
	STATUS status = STATUS_OK;
	for (Window * w : children)
	{
		status = w->render (pFbg);
		if (status != STATUS_OK)
			return status;
	}

	return status;
}

void Window::handleMouseInput(GLFWwindow *pWindow, int button, int action, int mods)
{
	double xpos, ypos;

	glfwGetCursorPos(pWindow, &xpos, &ypos);

	for (Window * w : children)
	{
		if (w->contains(xpos, ypos))
		{
			w->handleMouseInput(pWindow, button, action, mods);
			break;
		}
	}
}
