#pragma once

#include <cstdint>

// Bitwise indicators
#define BUTTON_IDLE 0x0
#define BUTTON_HOVER 0x1
#define BUTTON_DISABLED 0x2
#define BUTTON_ACTIVE 0x4

static inline uint32_t RGBMultiply(uint32_t rgb, float f)
{
    return (uint32_t((rgb & 0xFF0000) * f) & 0xFF0000) | (uint32_t((rgb & 0xFF00) * f) & 0xFF00) |
           (uint32_t((rgb & 0xFF) * f) & 0xFF);
}

struct Button;
struct GLFWwindow;

typedef void (*buttonHandleMouseInput)(Button&, GLFWwindow*, int, int, int);
typedef void (*buttonHandleMouseHover)(Button&, GLFWwindow*, double, double);
typedef void (*buttonHandleCheckDisable)(Button&, GLFWwindow*);

struct ButtonDefinition
{
    ButtonDefinition(buttonHandleMouseInput input, buttonHandleMouseHover hover,
        buttonHandleCheckDisable disable, uint32_t idleColor = 0x333333)
    {
        handleMouseInput = input;
        handleMouseHover = hover;
        handleCheckDisable = disable;

        colors[0] = idleColor;
        colors[1] = RGBMultiply(idleColor, 1.12f);
        colors[2] = 0x121212;
        colors[3] = 0x222222;
        colors[4] = RGBMultiply(idleColor, 1.4f);
        colors[5] = RGBMultiply(idleColor, 1.4f);
        colors[6] = 0x444444;
        colors[7] = 0x444444;
        colors[8] = 0xCCCC20;
        colors[9] = 0xFF8820;
    }

    buttonHandleMouseInput handleMouseInput;
    buttonHandleMouseHover handleMouseHover;
    buttonHandleCheckDisable handleCheckDisable;

    uint32_t colors[10];
};
