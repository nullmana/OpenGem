#include "fbgx.h"

#include "fbgraphics.h"

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) < (b) ? (b) : (a))

void fbgx_recta(struct _fbg *fbg, int x, int y, int w, int h, uint32_t r, uint32_t g, uint32_t b, uint32_t a) {
    int xx = 0, yy = 0, w3 = w * fbg->components;

    r &= 0xFF;
    g &= 0xFF;
    b &= 0xFF;
    a &= 0xFF;

    char *pix_pointer = (char *)(fbg->back_buffer + (y * fbg->line_length + x * fbg->components));

    for (yy = 0; yy < h; yy += 1) {
        for (xx = 0; xx < w; xx += 1) {
            *pix_pointer = ((a * r + (255 - a) * (uint32_t)(*pix_pointer)) >> 8);
            pix_pointer += 1;
            *pix_pointer = ((a * g + (255 - a) * (uint32_t)(*pix_pointer)) >> 8);
            pix_pointer += 1;
            *pix_pointer = ((a * b + (255 - a) * (uint32_t)(*pix_pointer)) >> 8);
            pix_pointer += 1;
            pix_pointer += fbg->comp_offset;
        }

        pix_pointer += (fbg->line_length - w3);
    }
}

void fbgx_tri(struct _fbg *fbg, int x1, int y1, int x2, int y2, int x3, int y3, uint8_t r, uint8_t g, uint8_t b) {
    int temp;
    int yy, xx;
    float m12 = 0.0f;
    float m23 = 0.0f;
    float m13 = 0.0f;
    float xa, xb;

    char *pix_pointer;

    if (x2 < x1)
    {
        temp = x1;
        x1 = x2;
        x2 = temp;
        temp = y1;
        y1 = y2;
        y2 = temp;
    }

    if (x3 < x1)
    {
        temp = x1;
        x1 = x3;
        x3 = temp;
        temp = y1;
        y1 = y3;
        y3 = temp;
    }

    if (x3 < x2)
    {
        temp = x2;
        x2 = x3;
        x3 = temp;
        temp = y2;
        y2 = y3;
        y3 = temp;
    }

    if (y2 != y1)
        m12 = ((float)(x2 - x1)) / (y2 - y1);
    if (y3 != y2)
        m23 = ((float)(x3 - x2)) / (y3 - y2);
    if (y3 != y1)
        m13 = ((float)(x3 - x1)) / (y3 - y1);

    if (y2 > y1 && y2 > y3)
    {
        if (y1 > y3) // 3 1 2
        {
            xa = x3;
            xb = x3;
            for (yy = y3; yy < y1; ++yy)
            {
                pix_pointer = (char *)(fbg->back_buffer + (yy * fbg->line_length + (int)(MIN(xa, xb)) * fbg->components));
                for (xx = MIN(xa, xb); xx < MAX(xa, xb); ++xx)
                {
                    *pix_pointer++ = r;
                    *pix_pointer++ = g;
                    *pix_pointer++ = b;
                    pix_pointer += fbg->comp_offset;
                }
                xa += m13;
                xb += m23;
            }

            xa = x1;
            for (yy = y1; yy < y2; ++yy)
            {
                pix_pointer = (char *)(fbg->back_buffer + (yy * fbg->line_length + (int)(MIN(xa, xb)) * fbg->components));
                for (xx = MIN(xa, xb); xx < MAX(xa, xb); ++xx)
                {
                    *pix_pointer++ = r;
                    *pix_pointer++ = g;
                    *pix_pointer++ = b;
                    pix_pointer += fbg->comp_offset;
                }
                xa += m12;
                xb += m23;
            }
        }
        else // 1 3 2
        {
            xa = x1;
            xb = x1;
            for (yy = y1; yy < y3; ++yy)
            {
                pix_pointer = (char *)(fbg->back_buffer + (yy * fbg->line_length + (int)(MIN(xa, xb)) * fbg->components));
                for (xx = MIN(xa, xb); xx < MAX(xa, xb); ++xx)
                {
                    *pix_pointer++ = r;
                    *pix_pointer++ = g;
                    *pix_pointer++ = b;
                    pix_pointer += fbg->comp_offset;
                }
                xa += m12;
                xb += m13;
            }

            xb = x3;
            for (yy = y3; yy < y2; ++yy)
            {
                pix_pointer = (char *)(fbg->back_buffer + (yy * fbg->line_length + (int)(MIN(xa, xb)) * fbg->components));
                for (xx = MIN(xa, xb); xx < MAX(xa, xb); ++xx)
                {
                    *pix_pointer++ = r;
                    *pix_pointer++ = g;
                    *pix_pointer++ = b;
                    pix_pointer += fbg->comp_offset;
                }
                xa += m12;
                xb += m23;
            }

        }
    }
    else if (y2 < y1 && y2 < y3)
    {
        if (y1 > y3) // 2 3 1
        {
            xa = x2;
            xb = x2;
            for (yy = y2; yy < y3; ++yy)
            {
                pix_pointer = (char *)(fbg->back_buffer + (yy * fbg->line_length + (int)(MIN(xa, xb)) * fbg->components));
                for (xx = MIN(xa, xb); xx < MAX(xa, xb); ++xx)
                {
                    *pix_pointer++ = r;
                    *pix_pointer++ = g;
                    *pix_pointer++ = b;
                    pix_pointer += fbg->comp_offset;
                }
                xa += m12;
                xb += m23;
            }

            xb = x3;
            for (yy = y3; yy < y1; ++yy)
            {
                pix_pointer = (char *)(fbg->back_buffer + (yy * fbg->line_length + (int)(MIN(xa, xb)) * fbg->components));
                for (xx = MIN(xa, xb); xx < MAX(xa, xb); ++xx)
                {
                    *pix_pointer++ = r;
                    *pix_pointer++ = g;
                    *pix_pointer++ = b;
                    pix_pointer += fbg->comp_offset;
                }
                xa += m12;
                xb += m13;
            }
        }
        else // 2 1 3
        {
            xa = x2;
            xb = x2;
            for (yy = y2; yy < y1; ++yy)
            {
                pix_pointer = (char *)(fbg->back_buffer + (yy * fbg->line_length + (int)(MIN(xa, xb)) * fbg->components));
                for (xx = MIN(xa, xb); xx < MAX(xa, xb); ++xx)
                {
                    *pix_pointer++ = r;
                    *pix_pointer++ = g;
                    *pix_pointer++ = b;
                    pix_pointer += fbg->comp_offset;
                }
                xa += m12;
                xb += m23;
            }

            xa = x1;
            for (yy = y1; yy < y3; ++yy)
            {
                pix_pointer = (char *)(fbg->back_buffer + (yy * fbg->line_length + (int)(MIN(xa, xb)) * fbg->components));
                for (xx = MIN(xa, xb); xx < MAX(xa, xb); ++xx)
                {
                    *pix_pointer++ = r;
                    *pix_pointer++ = g;
                    *pix_pointer++ = b;
                    pix_pointer += fbg->comp_offset;
                }
                xa += m13;
                xb += m23;
            }

        }
    }
    else
    {
        if (y1 > y3) // 3 2 1
        {
            xa = x3;
            xb = x3;
            for (yy = y3; yy < y2; ++yy)
            {
                pix_pointer = (char *)(fbg->back_buffer + (yy * fbg->line_length + (int)(MIN(xa, xb)) * fbg->components));
                for (xx = MIN(xa, xb); xx < MAX(xa, xb); ++xx)
                {
                    *pix_pointer++ = r;
                    *pix_pointer++ = g;
                    *pix_pointer++ = b;
                    pix_pointer += fbg->comp_offset;
                }
                xa -= m23;
                xb -= m13;
            }

            xa = x2;
            for (yy = y2; yy < y1; ++yy)
            {
                pix_pointer = (char *)(fbg->back_buffer + (yy * fbg->line_length + (int)(MIN(xa, xb)) * fbg->components));
                for (xx = MIN(xa, xb); xx < MAX(xa, xb); ++xx)
                {
                    *pix_pointer++ = r;
                    *pix_pointer++ = g;
                    *pix_pointer++ = b;
                    pix_pointer += fbg->comp_offset;
                }
                xa -= m12;
                xb -= m13;
            }
        }
        else // 1 2 3
        {
            xa = x1;
            xb = x1;
            for (yy = y1; yy < y2; ++yy)
            {
                pix_pointer = (char *)(fbg->back_buffer + (yy * fbg->line_length + (int)(MIN(xa, xb)) * fbg->components));
                for (xx = MIN(xa, xb); xx < MAX(xa, xb); ++xx)
                {
                    *pix_pointer++ = r;
                    *pix_pointer++ = g;
                    *pix_pointer++ = b;
                    pix_pointer += fbg->comp_offset;
                }
                xa += m13;
                xb += m12;
            }

            xb = x2;
            for (yy = y2; yy < y3; ++yy)
            {
                pix_pointer = (char *)(fbg->back_buffer + (yy * fbg->line_length + (int)(MIN(xa, xb)) * fbg->components));
                for (xx = MIN(xa, xb); xx < MAX(xa, xb); ++xx)
                {
                    *pix_pointer++ = r;
                    *pix_pointer++ = g;
                    *pix_pointer++ = b;
                    pix_pointer += fbg->comp_offset;
                }
                xa += m13;
                xb += m23;
            }
        }
    }
}
