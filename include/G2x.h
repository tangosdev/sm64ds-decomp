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
 * `unsigned short`, `s` is `short`, `i` is `int`, and `j` is `unsigned int`.
 *
 * SetBlendBrightness takes (vu16 *reg, int plane, int brightness), the Nitro
 * SDK's G2x_SetBlendBrightness_ signature. The symbol used to read `EPVtts`
 * (u16, s16); no ROM caller narrows either argument, the definition body
 * compiles identically with int, and the narrow spelling cost 8 to 16 bytes at
 * every caller passing a computed value.
 *
 * SetBlendAlpha takes (vu16 *reg, int plane1, int plane2, int ev1, int ev2),
 * the SDK's G2x_SetBlendAlpha_ signature (`EPVtiiii`). The symbol used to read
 * `EPVttttj`; the definition body compiles identically with int, and the u16
 * spelling narrowed every computed alpha (`0x10 - n`) with an lsl/lsr pair the
 * ROM does not have. The final stack argument is 32-bit because the ROM loads
 * it with `ldr`. It writes 32 bits through a `u16*`, so the store is cast in
 * the body rather than the pointer parameter being widened, which would also
 * change the mangling.
 */
#ifndef G2X_H
#define G2X_H
#include "types.h"

struct Matrix2x2;

struct G2x {
#ifdef __cplusplus
    /* All static -- see the header note. Unnamed: a named parameter takes an
       @NNN in every includer. The definitions in src/engine/gx/G2x.cpp name
       them: (reg, val, amt), (reg, firstTarget, secondTarget, firstAlpha,
       secondAlpha) and (reg, m, a, b, c, d). */
    static void SetBlendBrightness(volatile unsigned short *, int, int);
    static void SetBlendAlpha(volatile unsigned short *, int, int, int, int);
    static void SetBGyAffine(volatile unsigned short *, Matrix2x2 *, int, int, int, int);
#endif
};

#endif
