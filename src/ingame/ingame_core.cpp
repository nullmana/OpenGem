#include "ingame/ingame_core.h"

#include "wrapfbg.h"

STATUS IngameCore::init(struct _fbg* pFbg_)
{
    STATUS status = STATUS_OK;
    struct _fbg_glfw_context* pGlfwContext = (struct _fbg_glfw_context*)pFbg_->user_context;

    glfwSetWindowUserPointer(pGlfwContext->window, this);

    pFbg = pFbg_;

    status = inputHandler.init();
    if (status != STATUS_OK)
        return status;

    status = renderer.init(*this);
    if (status != STATUS_OK)
        return status;

    return status;
}
