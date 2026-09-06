//cpp
/* Minigame canvas line rasteriser (ov004 0x020ae5c4, 0x294 bytes). Walks a
 * Bresenham line from (x0, y0) to (x1, y1) and plots every point through the
 * canvas's virtual dScMgBase_c::Virtual88(x, y, colour, size) (vtable slot 34),
 * stepping along the major axis with the error term seeded at half the major
 * span; a zero-length line plots once. Callers: the trampoline minigame's
 * trajectory trace (src/actors/d_sc_mg_trampoline.cpp).
 *
 * Written against include/dScMgBase_c.h so the plot call is the real virtual
 * call: the ROM's ldr rN,[r0]; ldr rN,[rN,#0x88]; blx chain with the fifth
 * argument stored to the stack first is what the header's declaration
 * produces; a function-pointer temp evaluated before the call put the vtable
 * load before the stack argument and loaded it into a separate register.
 * The end row is copied into a local (yy1) that the vertical loop compares
 * against, the ROM's [sp, #0xc] slot; the horizontal loop compares x1 itself.
 * The two absolute values must be spelled as the copies first and the two
 * sign tests after (adx = dx; ady = dy; if (adx < 0) ...; if (ady < 0) ...):
 * that order colours dx/dy into r6/fp and lets the x-span test reuse the
 * flags of the dx subtraction (subs + rsbmi) while the y-span gets its own
 * cmp; testing them in the other order, or each right after its copy, swaps
 * the pair or the flag reuse. */
#include "dScMgBase_c.h"

extern "C" void func_ov004_020ae5c4(dScMgBase_c *thiz, int x0, int y0, int x1, int y1, int colour, int size)
{
    int dx, dy, adx, ady, err;
    int yy1;
    if (x0 == x1 && y0 == y1) {
        thiz->Virtual88(x0, y0, colour, size);
        return;
    }
    yy1 = y1;
    thiz->Virtual88(x0, y0, colour, size);
    dy = y1 - y0;
    dx = x1 - x0;
    adx = dx;
    ady = dy;
    if (adx < 0) adx = -adx;
    if (ady < 0) ady = -ady;
    if (adx >= ady) {
        err = adx / 2;
        for (;;) {
            if (dx == 0) {
                thiz->Virtual88(x0, y0, colour, size);
                return;
            }
            if (dx > 0) {
                err += ady;
                x0++;
                if (err > adx) {
                    if (dy >= 0) y0++; else y0--;
                    err -= adx;
                }
                thiz->Virtual88(x0, y0, colour, size);
                if (x0 == x1) return;
            } else {
                err += ady;
                x0--;
                if (err > adx) {
                    if (dy >= 0) y0++; else y0--;
                    err -= adx;
                }
                thiz->Virtual88(x0, y0, colour, size);
                if (x0 == x1) return;
            }
        }
    } else {
        err = ady / 2;
        for (;;) {
            if (dy == 0) {
                thiz->Virtual88(x0, y0, colour, size);
                return;
            }
            if (dy > 0) {
                err += adx;
                y0++;
                if (err > ady) {
                    if (dx >= 0) x0++; else x0--;
                    err -= ady;
                }
                thiz->Virtual88(x0, y0, colour, size);
                if (y0 == yy1) return;
            } else {
                err += adx;
                y0--;
                if (err > ady) {
                    if (dx >= 0) x0++; else x0--;
                    err -= ady;
                }
                thiz->Virtual88(x0, y0, colour, size);
                if (y0 == yy1) return;
            }
        }
    }
}
