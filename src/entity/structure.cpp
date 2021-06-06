#include "entity/structure.h"

Structure::Structure(int ix_, int iy_, int width_, int height_) : Targetable()
{
    ix = ix_;
    iy = iy_;
    width = width_;
    height = height_;

    isIndestructible = false;
}
