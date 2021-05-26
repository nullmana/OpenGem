#pragma once

#include "constants/status.h"

#include <vector>

struct _fbg;
struct GLFWwindow;

class Window
{
private:
	std::vector<Window*> children;
public:
	float x;
	float y;
	float width;
	float height;

	void addChildWindow(Window *pWindow) { children.push_back(pWindow); }

	virtual void resize(float x_, float y_, float w_, float h_)
	{
		x = x_;
		y = y_;
		width = w_;
		height = h_;
	}

	virtual bool contains(float xpos, float ypos)const
	{
		return (xpos > x) && (xpos < x+width) && (ypos > y) && (ypos < y+height);
	}

	virtual STATUS render(struct _fbg *pFbg);

	virtual void handleMouseInput(GLFWwindow *pWindow, int button, int action, int mods);
};
