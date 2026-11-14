//cpp
/* engine/gx/G2x.cpp -- the Nitro SDK's gx_g2.c (arm9 0x020551f0..0x0205532c)
 *
 * The 2D engine's blend/affine register writers, under the SDK's names with
 * C linkage (include/nitro/gx/g2.h; ntrtwl/NitroSDK libraries/gx/src/g2.c).
 * Each writes the registers behind the caller's `volatile u16 *`. The file
 * is written ROM-descending because mwccarm emits .text in reverse source
 * order under default deferred codegen.
 *
 * Four of the SDK file's five functions are here, in the SDK's order and at
 * the SDK's sizes. The fifth, G2x_SetBlendBrightnessExt_, is not in this
 * ROM: nothing calls it, so the linker dropped it.
 */
#include "nitro/gx/g2.h"
#include "types.h"

#include "Matrix2x2.h"

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
void G2x_SetBGyAffine_(volatile unsigned short *p, Matrix2x2 *m, int a, int b, int c, int d) {
    u16 pa = (u16)(s16)(m->m00 >> 4);
    u16 pb = (u16)(s16)(m->m01 >> 4);
    *(unsigned int *)p = pa | (pb << 16);

    u16 pc = (u16)(s16)(m->m10 >> 4);
    u16 pd = (u16)(s16)(m->m11 >> 4);
    *(unsigned int *)(p + 2) = pc | (pd << 16);

    int dx = c - a;
    int dy = d - b;
    int x = m->m00 * dx + m->m01 * dy + (a << 12);
    int y = m->m10 * dx + m->m11 * dy + (b << 12);

    *(int *)(p + 4) = x >> 4;
    *(int *)(p + 6) = y >> 4;
}

/* The final argument is 32-bit: the ROM loads its stack slot with `ldr`,
   not `ldrh`. */
void G2x_SetBlendAlpha_(volatile u16 *reg, int firstTarget, int secondTarget, int firstAlpha,
                        int secondAlpha) {
    *(u32 *)reg =
        ((firstTarget | 0x40) | (secondTarget << 8)) |
        ((firstAlpha | (secondAlpha << 8)) << 16);
}

/* Writes the hardware brightness registers behind `p`. The SIGN of `amt`
 * selects the mode rather than a separate flag: negative sets bits 0xc0
 * (fade to black) and writes the magnitude, positive sets 0x80 (fade to
 * white) and writes the value as-is. So one signed argument carries both
 * "which way" and "how much". */
void G2x_SetBlendBrightness_(volatile unsigned short *p, int val, int amt) {
    if (amt < 0) {
        p[0] = val | 0xc0;
        p[2] = -amt;
    } else {
        p[0] = val | 0x80;
        p[2] = amt;
    }
}

/* The first function of the SDK's file; the ROM puts it directly below
 * G2x_SetBlendBrightness_. It retunes a fade that
 * is already set up: the magnitude always goes to p[2], but the direction
 * bits in p[0] are rewritten only when the sign of `amt` reverses an existing
 * fade (0x80 to white, 0xc0 to black). A register with no fade selected keeps
 * its mode. */
void G2x_ChangeBlendBrightness_(volatile unsigned short *p, int amt) {
    unsigned short h = p[0];
    if (amt < 0) {
        if ((h & 0xc0) == 0x80) {
            p[0] = (h & ~0xc0) | 0xc0;
        }
        p[2] = (short)(-amt);
    } else {
        if ((h & 0xc0) == 0xc0) {
            p[0] = (h & ~0xc0) | 0x80;
        }
        p[2] = (short)amt;
    }
}
