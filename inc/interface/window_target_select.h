#pragma once

#include "interface/window.h"

class Gem;

class WindowTargetSelect : public Window
{
private:
    Gem* pTargetGem;
    int lastTargetOption;

    int cursorTargetOption(double xpos, double ypos) const;

public:
    WindowTargetSelect();

    virtual STATUS render(struct _fbg* pFbg);

    virtual void handleMouseInput(GLFWwindow* pWindow, int button, int action, int mods);
    virtual void handleMouseHover(GLFWwindow* pWindow, double xpos, double ypos);

    void openTargetSelect(Gem* pGem);
};
