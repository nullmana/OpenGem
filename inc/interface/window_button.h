#pragma once

#include "interface/button.h"
#include "interface/window.h"

#include <vector>

class WindowButton : public Window
{
private:
    std::vector<Button> buttons;
    int cols;

    int getButtonIndex(float xpos, float ypos) const;

public:
    WindowButton(const std::vector<ButtonDefinition>& buttonDefs, int cols_);

    virtual STATUS render(struct _fbg* pFbg);

    virtual void handleMouseInput(GLFWwindow* pWindow, int button, int action, int mods);
    virtual void handleMouseHover(GLFWwindow* pWindow, double xpos, double ypos);

    void setButtonActive(int index, bool active);
};
