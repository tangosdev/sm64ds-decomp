// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// MTX_RotX33_ @ 0x02052550 (arm9, Thumb): build 3x3 fx32 X-rotation matrix
// from (sin, cos): {1, 0, 0, 0, c, s, 0, -s, c} with 4096 = 1.0. Identified as
// the Nitro SDK's FX_mtx33.c MTX_RotX33_ from pret's FX_mtx33.c, whose body
// is the same instructions. Like its siblings MTX_RotY33_, MTX_RotZ33_ and
// MTX_RotX43_, it is Thumb assembly: `mov #1; lsl #12` builds 1.0 in a
// register, which compiled C only reproduced with a named `one` local held
// under opt_propagation off.
#include "math/Matrix.h"

#pragma thumb on
asm void MTX_RotX33_(Matrix3x3 *m, s32 s, s32 c) {
    mov r3, #1
    lsl r3, r3, #12
    str r3, [r0, #0]
    mov r3, #0
    str r3, [r0, #4]
    str r3, [r0, #8]
    str r3, [r0, #0xc]
    str r2, [r0, #0x10]
    str r1, [r0, #0x14]
    str r3, [r0, #0x18]
    neg r1, r1
    str r1, [r0, #0x1c]
    str r2, [r0, #0x20]
    bx lr
}
