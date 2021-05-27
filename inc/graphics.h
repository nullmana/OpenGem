#pragma once

#include <GL/glew.h>

#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#include "nanovg.h"
#include "nanovg_gl.h"

struct GraphicsContext
{
    GLFWwindow* win;
    NVGcontext* ctx;
};
