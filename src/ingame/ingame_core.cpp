#include "ingame/ingame_core.h"

#include "graphics.h"

STATUS IngameCore::init(GraphicsContext* context)
{
    STATUS status = STATUS_OK;

    glfwSetWindowUserPointer(context->win, this);

    pContext = context;

    status = inputHandler.init(*this);
    if (status != STATUS_OK)
        return status;

    status = renderer.init(*this);
    if (status != STATUS_OK)
        return status;

    return status;
}
