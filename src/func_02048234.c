#include "nitro/fx/fx.h"
typedef long long s64;
typedef signed short s16;

extern s16 data_02082214[];

void func_02048234(int *out, int sa, int sb, int sc, unsigned short ax, unsigned short ay, unsigned short az, int tx, int ty, int tz) {
    s16 *t = data_02082214;
    int ia = ax >> 4;
    int ib = ay >> 4;
    int ic = az >> 4;
    int a0 = t[ia * 2];
    int a1 = t[ia * 2 + 1];
    int b0 = t[ib * 2];
    int b1 = t[ib * 2 + 1];
    int c0 = t[ic * 2];
    int c1 = t[ic * 2 + 1];
    int p00 = (int)(((s64)a0 * c0) >> 12);
    int p11 = (int)(((s64)a1 * c1) >> 12);
    int p01 = (int)(((s64)a0 * c1) >> 12);
    int p10 = (int)(((s64)a1 * c0) >> 12);

    out[0] = (int)(((s64)sa * (((s64)b1 * c1) >> 12)) >> 12);
    out[1] = (int)(((s64)sa * (((s64)b1 * c0) >> 12)) >> 12);
    out[2] = (int)(((s64)(-b0) * sa) >> 12);
    out[3] = FX_MUL(sb, FX_MUL(b0, p01) - p10);
    out[4] = FX_MUL(sb, p11 + FX_MUL(b0, p00));
    out[5] = (int)(((s64)sb * (((s64)b1 * a0) >> 12)) >> 12);
    out[6] = FX_MUL(sc, p00 + FX_MUL(b0, p11));
    out[7] = FX_MUL(sc, FX_MUL(b0, p10) - p01);
    out[8] = (int)(((s64)sc * (((s64)b1 * a1) >> 12)) >> 12);
    out[9] = tx;
    out[10] = ty;
    out[11] = tz;
}
