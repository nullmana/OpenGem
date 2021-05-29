#pragma once

#include "interface/window.h"

class IngameManaPool;

class WindowMana : public Window
{
private:
    const IngameManaPool& manaPool;

public:
    WindowMana(const IngameManaPool& manaPool_) : manaPool(manaPool_) {}

    virtual STATUS render(struct _fbg* pFbg);
};
