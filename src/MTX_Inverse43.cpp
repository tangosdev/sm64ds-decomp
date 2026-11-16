//cpp
#include "types.h"
#include "nitro/fx/fx_mtx43.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/mi/memory.h"

extern "C" {

int MTX_Inverse43(const MtxFx43 *src, MtxFx43 *dst) {
    MtxFx43 buf;
    MtxFx43 *out;
    int p, q, r;
    int det;
    int recip;

    out = (src == dst) ? &buf : dst;

    p = (int)(((s64)src->m[1][1] * src->m[2][2] - (s64)src->m[1][2] * src->m[2][1] + 0x800) >> 12);
    q = (int)(((s64)src->m[1][0] * src->m[2][2] - (s64)src->m[1][2] * src->m[2][0] + 0x800) >> 12);
    r = (int)(((s64)src->m[1][0] * src->m[2][1] - (s64)src->m[1][1] * src->m[2][0] + 0x800) >> 12);

    det = (int)(((s64)src->m[0][0] * p - (s64)src->m[0][1] * q + (s64)src->m[0][2] * r + 0x800) >>
                12);
    if (det == 0) {
        return -1;
    }

    FX_InvAsync(det);

    {
        int x1 = (int)(((s64)src->m[0][1] * src->m[2][2] - (s64)src->m[2][1] * src->m[0][2]) >> 12);
        int x2 = (int)(((s64)src->m[0][1] * src->m[1][2] - (s64)src->m[1][1] * src->m[0][2]) >> 12);
        int x3 = (int)(((s64)src->m[0][0] * src->m[2][2] - (s64)src->m[2][0] * src->m[0][2]) >> 12);
        int x4 = (int)(((s64)src->m[0][0] * src->m[1][2] - (s64)src->m[1][0] * src->m[0][2]) >> 12);

        recip = FX_GetDivResult();

        out->m[0][0] = (int)(((s64)recip * p) >> 12);
        out->m[0][1] = -(int)(((s64)recip * x1) >> 12);
        out->m[0][2] = (int)(((s64)recip * x2) >> 12);
        out->m[1][0] = -(int)(((s64)recip * q) >> 12);
        out->m[1][1] = (int)(((s64)recip * x3) >> 12);
        out->m[1][2] = -(int)(((s64)recip * x4) >> 12);
        out->m[2][0] = (int)(((s64)recip * r) >> 12);
    }

    {
        int x5 = (int)(((s64)src->m[0][0] * src->m[2][1] - (s64)src->m[2][0] * src->m[0][1]) >> 12);
        out->m[2][1] = -(int)(((s64)recip * x5) >> 12);
    }
    {
        int x6 = (int)(((s64)src->m[0][0] * src->m[1][1] - (s64)src->m[1][0] * src->m[0][1]) >> 12);
        out->m[2][2] = (int)(((s64)recip * x6) >> 12);
    }

    out->m[3][0] = -(int)((((s64)out->m[0][0] * src->m[3][0]) + ((s64)out->m[1][0] * src->m[3][1]) +
                           ((s64)out->m[2][0] * src->m[3][2])) >>
                          12);
    out->m[3][1] = -(int)((((s64)out->m[0][1] * src->m[3][0]) + ((s64)out->m[1][1] * src->m[3][1]) +
                           ((s64)out->m[2][1] * src->m[3][2])) >>
                          12);
    out->m[3][2] = -(int)((((s64)out->m[0][2] * src->m[3][0]) + ((s64)out->m[1][2] * src->m[3][1]) +
                           ((s64)out->m[2][2] * src->m[3][2])) >>
                          12);

    if (out == &buf) {
        MI_Copy48B(&buf, dst);
    }

    return 0;
}
}
