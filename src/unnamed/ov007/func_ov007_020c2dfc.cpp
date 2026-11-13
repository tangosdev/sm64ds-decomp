//cpp
#include "Ov007StrokePath.h"
#include "nitro/fx/fx_cp.h"



/* Whether the path turns by less than v degrees at point idx - 1
 * (2 <= idx < pointCount): the change between the angles of segments idx - 1
 * and idx, squared, against v's binary angle squared. A negative v never
 * merges. */
int func_ov007_020c2dfc(Ov007StrokePath *path, int idx, int v) {
    int q;
    unsigned short *s;
    int d;
    if (v >= 0 && idx >= 2 && idx < path->pointCount) {
        q = (unsigned short)FX_DivS32(v << 0x10, 0x168);
        s = path->segmentAngles;
        d = (short)(s[idx] - s[idx - 1]);
        return d * d < q * q;
    }
    return 0;
}

