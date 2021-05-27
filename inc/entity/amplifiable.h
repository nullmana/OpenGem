#pragma once

#include <list>

class Amplifiable
{
public:
    Amplifiable(int ix_, int iy_) : ix(ix_), iy(iy_) {}

    std::list<Amplifiable*> amplifying;

    int ix;
    int iy;
};
