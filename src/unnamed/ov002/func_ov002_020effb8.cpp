//cpp
/* Four free C helpers between dPathLiftActor_c (0x020ef320..0x020effb8)
 * and daSCoin_c (0x020f03c4..):
 *
 * - func_ov002_020effb8: a uniform cubic B-spline through four Vector3
 *   control points. Its one caller is arm9 func_02008b4c, the path
 *   interpolator.
 * - func_ov002_020f02c8, func_ov002_020f030c and func_ov002_020f035c: the
 *   per-surface slope tables Player.h describes (slide acceleration, slide
 *   loss factor, slope threshold). Player, daObjBlockS_c and
 *   daObjMarioCap_c call them.
 *
 * Not folded: neither neighbour calls or is called by any of the four, so
 * neither dPathLiftActor_c nor daSCoin_c is evidence of an owner. All four
 * keep C linkage because those other TUs call them by their C names. */
#include "nitro/fx/fx.h"
#include "cstd.h"
#include "math/CalcLib.h"

extern "C" {

/* One coordinate's share: p * w, rounded from fx16 back to fx12. */
#define TERM(p, w) ((FX_MUL(p, w) + 8) >> 4)

/* Returns whether the floor normal's Y (normalY) is at most the
 * threshold for surface class sel. */
int func_ov002_020f035c(unsigned int sel, int normalY) {
    int threshold;
    switch (sel) {
    case 4:
        threshold = 0xff0;
        break;
    case 5:
        threshold = 0xfff;
        break;
    case 3:
        threshold = 0xfc1;
        break;
    default:
        threshold = 0xf74;
        break;
    case 1:
    case 2:
        threshold = 0xf08;
        break;
    }
    return normalY <= threshold;
}

int func_ov002_020f030c(int sel) {
    switch (sel) {
    case 4:
    case 5:
        return 0xfae;
    case 3:
        return 0xf5c;
    default:
    case 0:
        return 0xeb8;
    case 1:
    case 2:
        return 0xeb8;
    }
}

int func_ov002_020f02c8(int sel) {
    switch (sel) {
    case 4:
    case 5:
        return 0xa000;
    case 3:
        return 0x8000;
    case 0:
    default:
        return 0x7000;
    case 1:
    case 2:
        return 0x5000;
    }
}

/* out = the B-spline point at t (fx12, clamped to 1.0) on the segment
 * p1..p2, with p0 and p3 as the outer control points. The weights are
 * u^3/6, (3t^3 - 6t^2 + 4)/6, (-3t^3 + 3t^2 + 3t + 1)/6 and t^3/6 with
 * u = 1 - t, computed in fx16. Four equal points return p0 as is. */
// @symbol func_ov002_020effb8
void func_ov002_020effb8(Vector3 *out, s32 t, Vector3 *p0, Vector3 *p1, Vector3 *p2, Vector3 *p3) {
    s32 halfT3, rest;
    s32 t16, u, u16;
    s32 tSq, tCube, uSq, uCube;
    s32 w0, w1, w2, w3;

    if (Vec3_Equal(p0, p1) != 0 && Vec3_Equal(p1, p2) != 0 && Vec3_Equal(p2, p3) != 0) {
        out->x = p0->x;
        out->y = p0->y;
        out->z = p0->z;
        return;
    }
    if (t > 0x1000)
        t = 0x1000;
    t16 = t << 4;
    tSq = FX_MUL(t16, t);
    u = 0x1000 - t;
    tCube = FX_MUL(tSq, t);
    u16 = u << 4;
    uSq = FX_MUL(u16, u);
    uCube = FX_MUL(uSq, u);
    halfT3 = (tCube + 1) >> 1;
    w0 = cstd::fdiv(uCube, 0x6000);
    w3 = cstd::fdiv(tCube, 0x6000);
    rest = ((tSq + 1) >> 1) - halfT3;
    w2 = (rest + ((t16 + 1) >> 1)) + 0x2aab;
    w1 = (halfT3 - tSq) + 0xaaab;

    out->x = TERM(p0->x, w0) + TERM(p1->x, w1) + TERM(p2->x, w2) + TERM(p3->x, w3);
    out->y = TERM(p0->y, w0) + TERM(p1->y, w1) + TERM(p2->y, w2) + TERM(p3->y, w3);
    out->z = TERM(p0->z, w0) + TERM(p1->z, w1) + TERM(p2->z, w2) + TERM(p3->z, w3);
}
}
