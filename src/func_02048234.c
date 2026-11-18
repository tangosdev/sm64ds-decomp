#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "math/Matrix.h"
typedef long long s64;

void func_02048234(Matrix4x3 *out, int sa, int sb, int sc, unsigned short ax, unsigned short ay,
                   unsigned short az, int tx, int ty, int tz) {
    int ia = ax >> 4;
    int ib = ay >> 4;
    int ic = az >> 4;
    int a0 = FX_SinIdx(ia);
    int a1 = FX_CosIdx(ia);
    int b0 = FX_SinIdx(ib);
    int b1 = FX_CosIdx(ib);
    int c0 = FX_SinIdx(ic);
    int c1 = FX_CosIdx(ic);
    int p00 = (((s64)a0 * c0) >> 12);
    int p11 = (((s64)a1 * c1) >> 12);
    int p01 = (((s64)a0 * c1) >> 12);
    int p10 = (((s64)a1 * c0) >> 12);

    out->r.m[0][0] = (((s64)sa * (((s64)b1 * c1) >> 12)) >> 12);
    out->r.m[0][1] = (((s64)sa * (((s64)b1 * c0) >> 12)) >> 12);
    out->r.m[0][2] = (((s64)(-b0) * sa) >> 12);
    out->r.m[1][0] = FX_MUL(sb, FX_MUL(b0, p01) - p10);
    out->r.m[1][1] = FX_MUL(sb, p11 + FX_MUL(b0, p00));
    out->r.m[1][2] = (((s64)sb * (((s64)b1 * a0) >> 12)) >> 12);
    out->r.m[2][0] = FX_MUL(sc, p00 + FX_MUL(b0, p11));
    out->r.m[2][1] = FX_MUL(sc, FX_MUL(b0, p10) - p01);
    out->r.m[2][2] = (((s64)sc * (((s64)b1 * a1) >> 12)) >> 12);
    out->t.x = tx;
    out->t.y = ty;
    out->t.z = tz;
}
