/* Hand-edited, against evidence. This file used to carry the
 * "AUTO-GENERATED ... by tools/gen_header.py" banner and three fields
 * (unk_002, unk_004, unk_006). Those were never fields.
 *
 * G2x HAS NO INSTANCE STATE. Every one of its functions takes a
 * `volatile unsigned short*` as its FIRST parameter and writes the hardware
 * registers behind it; none takes a `this`. The ROM says so plainly -- r0 is
 * the destination in all three:
 *
 *     SetBlendBrightness   strh r1, [r0]    strh r?, [r0, #4]
 *     SetBlendAlpha        str  r1, [r0]
 *     SetBGyAffine         writes [r0], [r0,#4], [r0,#8], [r0,#12]
 *
 * The generator saw those register offsets and recorded them as this-relative
 * fields. They are offsets into the CALLER'S pointer, so the struct is empty
 * and the methods are static -- the same shape, and for the same reason, as
 * include/OAM.h.
 *
 * PARAMETER TYPES ARE READ OFF THE MANGLED NAME, not chosen. That matters here
 * more than usual: a wrong width names a symbol that exists nowhere, and this
 * class has already cost one ROM-build failure that way (a shadow `struct G2x`
 * with an `int` parameter). `PVt` is `volatile unsigned short*`, `t` is
 * `unsigned short`, `s` is `short`, `i` is `int`.
 *
 * Note SetBlendAlpha takes FIVE parameters (`EPVttttt`), not the six its
 * pre-image declared, and writes 32 bits through a `u16*` -- so the store is
 * cast in the body rather than the parameter being widened, which would change
 * the mangling.
 */
#ifndef G2X_H
#define G2X_H
#include "types.h"

struct Matrix2x2;

struct G2x {
#ifdef __cplusplus
    /* All static -- see the header note. */
    static void SetBlendBrightness(volatile unsigned short *p, unsigned short val, short amt);
    static void SetBlendAlpha(volatile unsigned short *reg, unsigned short a,
                              unsigned short b, unsigned short c, unsigned short d);
    static void SetBGyAffine(volatile unsigned short *p, Matrix2x2 *m,
                             int a, int b, int c, int d);
#endif
};

#endif
