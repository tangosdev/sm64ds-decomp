//cpp
// @symbol _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii
/* recovered: shared header, real C++ method (static)
 *
 * Loads a background's affine transform into the hardware registers behind `p`.
 *
 * The four matrix terms go out as TWO 32-bit writes, packed in pairs, each term
 * narrowed to s16 after a >> 4 -- the registers are 1.7.8 fixed point while the
 * matrix is 20.12, so the shift is the format conversion, not a scale.
 *
 * The reference point is the interesting part: dx/dy are the offset from (a,b)
 * to (c,d), and the displacement written is `m * d + (origin << 12)`. So the
 * caller gives a point to rotate ABOUT and a point to rotate TO, and this
 * resolves them into the single origin the hardware actually takes.
 *
 * No `this`: r0 is the register pointer. See include/G2x.h.
 */
#include "G2x.h"
#include "types.h"

struct Matrix2x2 { int m[4]; };

void G2x::SetBGyAffine(volatile unsigned short *p, Matrix2x2 *m, int a, int b, int c, int d)
{
    u16 pa = (u16)(s16)(m->m[0] >> 4);
    u16 pb = (u16)(s16)(m->m[1] >> 4);
    *(volatile unsigned int *)p = pa | (pb << 16);

    u16 pc = (u16)(s16)(m->m[2] >> 4);
    u16 pd = (u16)(s16)(m->m[3] >> 4);
    *(volatile unsigned int *)(p + 2) = pc | (pd << 16);

    int dx = c - a;
    int dy = d - b;
    int x = m->m[0] * dx + m->m[1] * dy + (a << 12);
    int y = m->m[2] * dx + m->m[3] * dy + (b << 12);

    *(volatile int *)(p + 4) = x >> 4;
    *(volatile int *)(p + 6) = y >> 4;
}
