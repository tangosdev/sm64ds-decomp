//cpp
#include "types.h"
#include "nitro/cp/sqrt.h"


inline s64 inline_fn(int arg0) {
    return (s64)arg0;
}

int func_ov007_020c31e0(int *xa, int *xb, int n, int cx, int qy, int maxr, int *out) {
    int i;
    int best = -1;
    s64 bestd = 0x7fffffffffffffffLL;
    s64 d;
    int dx;
    int dy;
    for (i = 0; i < n; i++) {
        dy = qy - xb[i];
        dx = cx - xa[i];
        d = (inline_fn(dy) * dy) + (((s64)dx) * dx);
        if (d < bestd) {
            best = i;
            bestd = d;
        }
    }

    if ((maxr < 0) || ((inline_fn(maxr) * maxr) >= bestd)) {
        CP_SetSqrt64((u64)bestd << 2);
        if (out) {
            *out = ((s32)CP_GetSqrtResult32() + 1) >> 1;
        }
        return best;
    }
    return -1;
}

