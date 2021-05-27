#pragma once

#include "constants/status.h"

#include <vector>

struct GraphicsContext;

class Window
{
private:
    std::vector<Window*> children;
    Window* pParent;

public:
    Window();

    float x;
    float y;
    float width;
    float height;

    void addChildWindow(Window* pWindow)
    {
        children.push_back(pWindow);
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

    virtual STATUS render(struct GraphicsContext* pFbg);

    virtual void handleMouseInput(
        struct GraphicsContext* pWindow, int button, int action, int mods);
};
