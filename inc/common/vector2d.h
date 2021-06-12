#pragma once

#include <cstddef>
#include <vector>

template <class T>
class vector2d
{
private:
    std::vector<T> data;
    size_t h;
    size_t w;

public:
    vector2d(size_t h_, size_t w_)
        : data(h_ * w_), h(h_), w(w_) {}
    vector2d(size_t h_, size_t w_, T value)
        : data(h_ * w_, value), h(h_), w(w_) {}

    T& operator[](size_t pos) { return data[pos]; }
    const T& operator[](size_t pos) const { return data[pos]; }

    T& at(size_t y, size_t x) { return data.at(y * w + x); }
    const T& at(size_t y, size_t x) const { return data.at(y * w + x); }

    void clear()
    {
        data.clear();
        data.resize(h * w);
    }
    void clear(T value)
    {
        data.clear();
        data.resize(h * w, value);
    }
};
