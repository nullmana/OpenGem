#pragma once

#include <stdint.h>

struct _fbg;

extern void fbgx_recta(struct _fbg *fbg, int x, int y, int w, int h, uint32_t r, uint32_t g, uint32_t b, uint32_t a);

extern void fbgx_tri(struct _fbg *fbg, int x1, int y1, int x2, int y2, int x3, int y3, uint8_t r, uint8_t g, uint8_t b);
