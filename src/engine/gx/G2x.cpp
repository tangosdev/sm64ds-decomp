//cpp
/* engine/gx/G2x.cpp — G2x TU (arm9 0x02055238..0x0205532c)
 *
 * G2x has no instance state: every member is static and writes the 2D
 * engine's blend/affine registers through the caller's `volatile u16*`
 * (include/G2x.h). The file is written ROM-descending because mwccarm
 * emits .text in reverse source order under default deferred codegen.
 */
#include "G2x.h"
#include "types.h"

struct Matrix2x2 { int m[4]; };

// @symbol _ZN3G2x12SetBGyAffineEPVtP9Matrix2x2iiii
/* Loads a background's affine transform into the hardware registers behind `p`.
 *
 * The four matrix terms go out as TWO 32-bit writes, packed in pairs, each term
 * narrowed to s16 after a >> 4 -- the registers are 1.7.8 fixed point while the
 * matrix is 20.12, so the shift is the format conversion, not a scale.
 *
 * The reference point is the interesting part: dx/dy are the offset from (a,b)
 * to (c,d), and the displacement written is `m * d + (origin << 12)`. So the
 * caller gives a point to rotate ABOUT and a point to rotate TO, and this
 * resolves them into the single origin the hardware actually takes. */
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

// @symbol _ZN3G2x13SetBlendAlphaEPVtiiii
/* The Nitro SDK's G2x_SetBlendAlpha_(vu16 *, int, int, int, int). The final
   argument is 32-bit: the ROM loads its stack slot with `ldr`, not `ldrh`. */
void G2x::SetBlendAlpha(volatile u16 *reg, int firstTarget, int secondTarget, int firstAlpha,
                        int secondAlpha) {
    *(volatile u32 *)reg =
        ((firstTarget | 0x40) | (secondTarget << 8)) |
        ((firstAlpha | (secondAlpha << 8)) << 16);
}

// @symbol _ZN3G2x18SetBlendBrightnessEPVtii
/* Writes the hardware brightness registers behind `p`. The SIGN of `amt`
 * selects the mode rather than a separate flag: negative sets bits 0xc0
 * (fade to black) and writes the magnitude, positive sets 0x80 (fade to
 * white) and writes the value as-is. So one signed argument carries both
 * "which way" and "how much". */
void G2x::SetBlendBrightness(volatile unsigned short *p, int val, int amt) {
    if (amt < 0) {
        p[0] = val | 0xc0;
        p[2] = -amt;
    } else {
        p[0] = val | 0x80;
        p[2] = amt;
    }
}
