#pragma once

#include "interface/button_definition.h"

#include <cstring>

struct Button
{
    Button(const ButtonDefinition& d)
    {
        handleMouseInput = d.handleMouseInput;
        handleMouseHover = d.handleMouseHover;
        memcpy(color, d.colors, sizeof(color));
        state = BUTTON_IDLE;
        forceColor = -1;
    }

    buttonHandleMouseInput handleMouseInput;
    buttonHandleMouseHover handleMouseHover;

    uint32_t color[10];
    int state;
    int forceColor;
};
