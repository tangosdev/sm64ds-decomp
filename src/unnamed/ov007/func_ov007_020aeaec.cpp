//cpp
#include "Ov007Item.h"
#include "nitro/fx/fx_cp.h"
#include "math/ProgressRatio.h"



/* How far the item's current state has run between ticks lo and hi, as Q12
 * (0 at or before lo, 0x1000 at or after hi); flag reverses it. */
int func_ov007_020aeaec(Ov007Item *item, int flag, int lo, int hi) {
    int r = RangeRatio(item->state->stateTicks, lo, hi);
    if (flag)
        r = 0x1000 - r;
    return r;
}

