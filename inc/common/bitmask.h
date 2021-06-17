#pragma once

#include <cstdint>

inline int bitmapCountOnes(uint32_t i)
{
    int count = 0;
    while (i != 0)
    {
        i &= (i - 1);
        ++count;
    }
    return count;
}
