#pragma once

#include <cstdint>

// Bitwise indicators
#define BUTTON_IDLE 0x0
#define BUTTON_HOVER 0x1
#define BUTTON_DISABLED 0x2
#define BUTTON_ACTIVE 0x4

struct Button;
struct GLFWwindow;

typedef void (*buttonHandleMouseInput)(Button&, GLFWwindow*, int, int, int);
typedef void (*buttonHandleMouseHover)(Button&, GLFWwindow*, double, double);

struct ButtonDefinition
{
    ButtonDefinition(buttonHandleMouseInput input, buttonHandleMouseHover hover)
    {
        handleMouseInput = input;
        handleMouseHover = hover;

        colors[0] = 0x333333;
        colors[1] = 0x484848;
        colors[2] = 0x121212;
        colors[3] = 0x222222;
        colors[4] = 0xAAAAAA;
        colors[5] = 0xAAAAAA;
        colors[6] = 0xAAAAAA;
        colors[7] = 0xAAAAAA;
    }

    buttonHandleMouseInput handleMouseInput;
    buttonHandleMouseHover handleMouseHover;

    uint32_t colors[8];
};
