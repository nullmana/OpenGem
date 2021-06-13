#include "interface/window_button.h"

#include "ingame/ingame_core.h"

#include "wrapfbg.h"

WindowButton::WindowButton(const std::vector<ButtonDefinition>& buttonDefs, int cols_)
{
    cols = cols_;

    for (const ButtonDefinition& d : buttonDefs)
    {
        buttons.emplace_back(d);
    }
}

STATUS WindowButton::render(struct _fbg* pFbg)
{
    struct _fbg_glfw_context* pGlfwContext = (struct _fbg_glfw_context*)pFbg->user_context;

    fbg_rect(pFbg, x - 2, y - 2, width + 4, height + 4, 0x1D, 0x22, 0x2E);

    float hscale = width / float(cols);
    float vscale = height / float(buttons.size() / cols);

    for (int i = 0; i < buttons.size(); ++i)
    {
        Button& b = buttons[i];
        if (b.handleCheckDisable != NULL)
            b.handleCheckDisable(b, pGlfwContext->window);

        const uint32_t& color = b.color[(b.forceColor == -1) ? b.state : b.forceColor];

        fbg_rect(pFbg, (i % cols) * hscale + x, (i / cols) * vscale + y, hscale, vscale,
            (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    }

    return STATUS_OK;
}

int WindowButton::getButtonIndex(float xpos, float ypos) const
{
    float relX = (xpos - x) / width;
    float relY = (ypos - y) / height;

    return cols * int(relY * (buttons.size() / cols)) + int(relX * cols);
}

void WindowButton::handleMouseInput(GLFWwindow* pWindow, int button, int action, int mods)
{
    double xpos, ypos;

    glfwGetCursorPos(pWindow, &xpos, &ypos);

    int buttonIndex = getButtonIndex(xpos, ypos);

    if (buttons[buttonIndex].handleMouseInput != NULL)
        buttons[buttonIndex].handleMouseInput(buttons[buttonIndex], pWindow, button, action, mods);
}

void WindowButton::handleMouseHover(GLFWwindow* pWindow, double xpos, double ypos)
{
    for (Button& b : buttons)
        b.state &= ~BUTTON_HOVER;

    if (contains(xpos, ypos))
    {
        int buttonIndex = getButtonIndex(xpos, ypos);

        if (buttons[buttonIndex].handleMouseHover != NULL)
            buttons[buttonIndex].handleMouseHover(buttons[buttonIndex], pWindow, xpos, ypos);
    }
}

void WindowButton::setButtonActive(int index, bool active)
{
    if (active)
        buttons[index].state |= BUTTON_ACTIVE;
    else
        buttons[index].state &= ~BUTTON_ACTIVE;
}
