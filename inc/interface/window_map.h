#pragma once

#include "interface/window.h"

class IngameMap;

class WindowMap : public Window
{
private:
	IngameMap &map;

	int dragStartX;
	int dragStartY;
public:
	WindowMap(IngameMap &map_, float x_, float y_, float w_, float h_) : map(map_), Window(x_, y_, w_, h_)
	{
		dragStartX = 0;
		dragStartY = 0;
	}

	virtual STATUS render(struct _fbg *pFbg);
	virtual void handleMouseInput(GLFWwindow *pWindow, int button, int action, int mods);
};
