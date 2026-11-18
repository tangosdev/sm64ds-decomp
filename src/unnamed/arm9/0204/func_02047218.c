#include "nitro/fx/fx.h"
#include "math/Matrix.h"

/* Add t times the product m1 m0 of two 4x3 matrices (row vectors: m1,
   then m0) into dst: each row of m1 is taken through m0's rotation, and
   the translation row adds m0's translation too. func_02044e50 sums one
   such term per weight, with byte weights shifted up to fx32, to build a
   blended matrix. */
void func_02047218(const Matrix4x3 *m0, const Matrix4x3 *m1, Matrix4x3 *dst, fx32 t) {
    fx32 s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;

    s1 = FX_MUL(m0->r.m[0][1], t);
    s2 = FX_MUL(m0->r.m[0][2], t);
    s4 = FX_MUL(m0->r.m[1][1], t);
    s5 = FX_MUL(m0->r.m[1][2], t);
    s7 = FX_MUL(m0->r.m[2][1], t);
    s8 = FX_MUL(m0->r.m[2][2], t);
    s9 = FX_MUL(m0->t.x, t);
    s10 = FX_MUL(m0->t.y, t);
    s11 = FX_MUL(m0->t.z, t);
    s3 = FX_MUL(m0->r.m[1][0], t);
    s6 = FX_MUL(m0->r.m[2][0], t);
    s0 = FX_MUL(m0->r.m[0][0], t);

    dst->r.m[0][0] +=
        FX_MUL(m1->r.m[0][0], s0) + (FX_MUL(m1->r.m[0][2], s6) + FX_MUL(m1->r.m[0][1], s3));
    dst->r.m[1][0] +=
        FX_MUL(m1->r.m[1][0], s0) + (FX_MUL(m1->r.m[1][2], s6) + FX_MUL(m1->r.m[1][1], s3));
    dst->r.m[2][0] +=
        FX_MUL(m1->r.m[2][0], s0) + (FX_MUL(m1->r.m[2][2], s6) + FX_MUL(m1->r.m[2][1], s3));
    dst->t.x += s9 + (FX_MUL(m1->t.x, s0) + (FX_MUL(m1->t.z, s6) + FX_MUL(m1->t.y, s3)));
    dst->r.m[0][1] +=
        FX_MUL(m1->r.m[0][0], s1) + (FX_MUL(m1->r.m[0][2], s7) + FX_MUL(m1->r.m[0][1], s4));
    dst->r.m[1][1] +=
        FX_MUL(m1->r.m[1][0], s1) + (FX_MUL(m1->r.m[1][2], s7) + FX_MUL(m1->r.m[1][1], s4));
    dst->r.m[2][1] +=
        FX_MUL(m1->r.m[2][0], s1) + (FX_MUL(m1->r.m[2][2], s7) + FX_MUL(m1->r.m[2][1], s4));
    dst->t.y += s10 + (FX_MUL(m1->t.x, s1) + (FX_MUL(m1->t.z, s7) + FX_MUL(m1->t.y, s4)));
    dst->r.m[0][2] +=
        FX_MUL(m1->r.m[0][0], s2) + (FX_MUL(m1->r.m[0][2], s8) + FX_MUL(m1->r.m[0][1], s5));
    dst->r.m[1][2] +=
        FX_MUL(m1->r.m[1][0], s2) + (FX_MUL(m1->r.m[1][2], s8) + FX_MUL(m1->r.m[1][1], s5));
    dst->r.m[2][2] +=
        FX_MUL(m1->r.m[2][0], s2) + (FX_MUL(m1->r.m[2][2], s8) + FX_MUL(m1->r.m[2][1], s5));
    dst->t.z += s11 + (FX_MUL(m1->t.x, s2) + (FX_MUL(m1->t.z, s8) + FX_MUL(m1->t.y, s5)));
}
