#pragma once

#include "constants/status.h"

struct _fbg;
struct GLFWwindow;

class Window
{
public:
	float x;
	float y;
	float width;
	float height;

	Window(float x_, float y_, float w_, float h_) : x(x_), y(y_), width(w_), height(h_) {}

	virtual void resize(float x_, float y_, float w_, float h_)
	{
		x = x_;
		y = y_;
		width = w_;
		height = h_;
	}

	virtual bool contains(float xpos, float ypos)
	{
		return xpos > x && xpos < x+width && ypos > y && ypos < y+height;
	}

	virtual STATUS render(struct _fbg *pFbg)=0;

	virtual void handleMouseInput(GLFWwindow *pWindow, int button, int action, int mods)=0;
};
