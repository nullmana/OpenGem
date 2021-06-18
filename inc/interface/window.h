#pragma once

#include "constants/status.h"

#include <map>
#include <vector>

struct _fbg;
struct GLFWwindow;

class Window
{
protected:
    std::vector<Window*> children;
    std::map<int, Window*> childRenderOrder;
    Window* pParent;

public:
    Window();

    float x;
    float y;
    float width;
    float height;

    void addChildWindow(Window* pWindow, int renderOrder)
    {
        children.push_back(pWindow);
        childRenderOrder[renderOrder] = pWindow;
        pWindow->setParent(this);
    }
    void setParent(Window* pWindow) { pParent = pWindow; }
    Window* getParent() const { return pParent; }
    const Window* getRootWindow() const;

    virtual void resize(float x_, float y_, float w_, float h_)
    {
        x = x_;
        y = y_;
        width = w_;
        height = h_;
    }

    virtual bool contains(float xpos, float ypos) const
    {
        return (xpos > x) && (xpos < x + width) && (ypos > y) && (ypos < y + height);
    }

    virtual STATUS render(struct _fbg* pFbg);

    // Window mouse input is sent only to child window containing the action
    virtual void handleMouseInput(GLFWwindow* pWindow, int button, int action, int mods);

    // Window keyboard input is sent only to child window currently containing the pointer
    virtual void handleKeyInput(GLFWwindow* pWindow, int key, int scancode, int action, int mods);

    // Window mouse hover is sent to ALL child windows
    virtual void handleMouseHover(GLFWwindow* pWindow, double xpos, double ypos);
};
