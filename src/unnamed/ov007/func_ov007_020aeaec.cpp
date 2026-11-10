//cpp
#include "Ov007Item.h"
#include "nitro/fx/fx_cp.h"

extern "C" {

/* How far the item's current state has run between ticks lo and hi, as Q12
 * (0 at or before lo, 0x1000 at or after hi); flag reverses it. */
int func_ov007_020aeaec(Ov007Item *item, int flag, int lo, int hi) {
    int ticks = item->state->stateTicks;
    int r;
    if (ticks <= lo)
        r = 0;
    else if (ticks >= hi)
        r = 0x1000;
    else
        r = FX_DivS32((ticks - lo) << 0xc, hi - lo);
    if (flag)
        r = 0x1000 - r;
    return r;
}
}
