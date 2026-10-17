#include "nitro/fx/fx.h"

void func_02047218(fx32 *m0, fx32 *m1, fx32 *dst, fx32 t)
{
    fx32 s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;

    s1 = FX_MUL(m0[1], t);
    s2 = FX_MUL(m0[2], t);
    s4 = FX_MUL(m0[4], t);
    s5 = FX_MUL(m0[5], t);
    s7 = FX_MUL(m0[7], t);
    s8 = FX_MUL(m0[8], t);
    s9 = FX_MUL(m0[9], t);
    s10 = FX_MUL(m0[10], t);
    s11 = FX_MUL(m0[11], t);
    s3 = FX_MUL(m0[3], t);
    s6 = FX_MUL(m0[6], t);
    s0 = FX_MUL(m0[0], t);

    dst[0] += FX_MUL(m1[0], s0) + (FX_MUL(m1[2], s6) + FX_MUL(m1[1], s3));
    dst[3] += FX_MUL(m1[3], s0) + (FX_MUL(m1[5], s6) + FX_MUL(m1[4], s3));
    dst[6] += FX_MUL(m1[6], s0) + (FX_MUL(m1[8], s6) + FX_MUL(m1[7], s3));
    dst[9] += s9 + (FX_MUL(m1[9], s0) + (FX_MUL(m1[11], s6) + FX_MUL(m1[10], s3)));
    dst[1] += FX_MUL(m1[0], s1) + (FX_MUL(m1[2], s7) + FX_MUL(m1[1], s4));
    dst[4] += FX_MUL(m1[3], s1) + (FX_MUL(m1[5], s7) + FX_MUL(m1[4], s4));
    dst[7] += FX_MUL(m1[6], s1) + (FX_MUL(m1[8], s7) + FX_MUL(m1[7], s4));
    dst[10] += s10 + (FX_MUL(m1[9], s1) + (FX_MUL(m1[11], s7) + FX_MUL(m1[10], s4)));
    dst[2] += FX_MUL(m1[0], s2) + (FX_MUL(m1[2], s8) + FX_MUL(m1[1], s5));
    dst[5] += FX_MUL(m1[3], s2) + (FX_MUL(m1[5], s8) + FX_MUL(m1[4], s5));
    dst[8] += FX_MUL(m1[6], s2) + (FX_MUL(m1[8], s8) + FX_MUL(m1[7], s5));
    dst[11] += s11 + (FX_MUL(m1[9], s2) + (FX_MUL(m1[11], s8) + FX_MUL(m1[10], s5)));
}
