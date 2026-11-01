//cpp
#include "types.h"

extern "C" {
/* Fills rows 32-entry rows of a BG screen with the tile numbers 0, 1, 2...
 * in palette (palette & 0xf). */
void func_ov007_020c0308(u16 *screen, int rows, int palette) {
    u32 i = 0;
    u32 hi;
    u32 n;
    if ((u32)(rows << 5) == 0)
        return;
    hi = ((u32)(palette << 28)) >> 16;
    n = rows << 5;
    do {
        u16 lo = i;
        screen[i] = hi | lo;
        i++;
    } while (i < n);
}
}
