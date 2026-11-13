//cpp
#include "types.h"
#include "common.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/mi/memory.h"



int MTX_Inverse43(const Matrix4x3 *src, Matrix4x3 *dst) {
    Matrix4x3 buf;
    Matrix4x3 *out;
    int p, q, r;
    int det;
    int recip;

    out = (src == dst) ? &buf : dst;

    p = (int)(((s64)src->m[4] * src->m[8] - (s64)src->m[5] * src->m[7] + 0x800) >> 12);
    q = (int)(((s64)src->m[3] * src->m[8] - (s64)src->m[5] * src->m[6] + 0x800) >> 12);
    r = (int)(((s64)src->m[3] * src->m[7] - (s64)src->m[4] * src->m[6] + 0x800) >> 12);

    det = (int)(((s64)src->m[0] * p - (s64)src->m[1] * q + (s64)src->m[2] * r + 0x800) >> 12);
    if (det == 0) {
        return -1;
    }

    FX_InvAsync(det);

    {
        int x1 = (int)(((s64)src->m[1] * src->m[8] - (s64)src->m[7] * src->m[2]) >> 12);
        int x2 = (int)(((s64)src->m[1] * src->m[5] - (s64)src->m[4] * src->m[2]) >> 12);
        int x3 = (int)(((s64)src->m[0] * src->m[8] - (s64)src->m[6] * src->m[2]) >> 12);
        int x4 = (int)(((s64)src->m[0] * src->m[5] - (s64)src->m[3] * src->m[2]) >> 12);

        recip = FX_GetDivResult();

        out->m[0] = (int)(((s64)recip * p) >> 12);
        out->m[1] = -(int)(((s64)recip * x1) >> 12);
        out->m[2] = (int)(((s64)recip * x2) >> 12);
        out->m[3] = -(int)(((s64)recip * q) >> 12);
        out->m[4] = (int)(((s64)recip * x3) >> 12);
        out->m[5] = -(int)(((s64)recip * x4) >> 12);
        out->m[6] = (int)(((s64)recip * r) >> 12);
    }

    {
        int x5 = (int)(((s64)src->m[0] * src->m[7] - (s64)src->m[6] * src->m[1]) >> 12);
        out->m[7] = -(int)(((s64)recip * x5) >> 12);
    }
    {
        int x6 = (int)(((s64)src->m[0] * src->m[4] - (s64)src->m[3] * src->m[1]) >> 12);
        out->m[8] = (int)(((s64)recip * x6) >> 12);
    }

    out->m[9] = -(int)((((s64)out->m[0] * src->m[9]) + ((s64)out->m[3] * src->m[10]) +
                        ((s64)out->m[6] * src->m[11])) >>
                       12);
    out->m[10] = -(int)((((s64)out->m[1] * src->m[9]) + ((s64)out->m[4] * src->m[10]) +
                         ((s64)out->m[7] * src->m[11])) >>
                        12);
    out->m[11] = -(int)((((s64)out->m[2] * src->m[9]) + ((s64)out->m[5] * src->m[10]) +
                         ((s64)out->m[8] * src->m[11])) >>
                        12);

    if (out == &buf) {
        MI_Copy48B(&buf, dst);
    }

    return 0;
}
