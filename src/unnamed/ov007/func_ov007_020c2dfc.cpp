//cpp
#include "Ov007StrokePath.h"
#include "cstd.h"

extern "C" {

/* Whether the turn into point idx (2 <= idx < pointCount) is sharper than v
 * degrees: the change between the two segment angles before it, squared,
 * against v's binary angle squared. self keeps its char * spelling; `path`
 * is the typed view. */
int func_ov007_020c2dfc(char* self, int idx, int v) {
    Ov007StrokePath *path = (Ov007StrokePath *)self;
    int q;
    unsigned short* s;
    int d;
    if (v >= 0 && idx >= 2 && idx < path->pointCount) {
        q = (unsigned short)cstd::div(v << 0x10, 0x168);
        s = path->segmentAngles;
        d = (short)(s[idx] - s[idx - 1]);
        return d * d < q * q;
    }
    return 0;
}
}
