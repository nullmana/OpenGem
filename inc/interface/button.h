#pragma once

#include "interface/button_definition.h"

#include <cstring>

struct Button
{
    Button(const ButtonDefinition& d)
    {
        handleMouseInput = d.handleMouseInput;
        handleMouseHover = d.handleMouseHover;
        handleCheckDisable = d.handleCheckDisable;
        memcpy(color, d.colors, sizeof(color));
        state = BUTTON_IDLE;
        forceColor = -1;
    }

    buttonHandleMouseInput handleMouseInput;
    buttonHandleMouseHover handleMouseHover;
    buttonHandleCheckDisable handleCheckDisable;

    uint32_t color[10];
    int state;
    int forceColor;
};
