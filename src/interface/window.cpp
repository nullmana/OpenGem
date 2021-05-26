#include "interface/window.h"

#include "glfw/fbg_glfw.h"

Window::Window()
{
    pParent = NULL;
    x       = 0.0f;
    y       = 0.0f;
    width   = 0.0f;
    height  = 0.0f;
}

STATUS Window::render(struct _fbg* pFbg)
{
    STATUS status = STATUS_OK;
    for (Window* w : children)
    {
        status = w->render(pFbg);
        if (status != STATUS_OK)
            return status;
    }

    return status;
}

void Window::handleMouseInput(GLFWwindow* pWindow, int button, int action, int mods)
{
    double xpos, ypos;

    glfwGetCursorPos(pWindow, &xpos, &ypos);

    for (Window* w : children)
    {
        if (w->contains(xpos, ypos))
        {
            w->handleMouseInput(pWindow, button, action, mods);
            break;
        }
    }
}

const Window* Window::getRootWindow() const
{
    const Window* pRoot       = this;
    const Window* pRootParent = pParent;

    while (pRootParent != NULL)
    {
        pRoot       = pRootParent;
        pRootParent = pRoot->getParent();
    }

    return pRoot;
}
