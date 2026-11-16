// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// MTX_RotY33_ @ 0x0205256c (arm9, Thumb): build 3x3 fx32 Y-rotation matrix
// from (sin, cos): {c, 0, -s, 0, 1, 0, s, 0, c} with 4096 = 1.0. Identified as
// the Nitro SDK's FX_mtx33.c MTX_RotY33_ from pret's FX_mtx33.c, whose body
// is the same instructions.
#include "math/Matrix.h"

#pragma thumb on
asm void MTX_RotY33_(Matrix3x3 *m, int s, int c) {
    str r2, [r0, #0]
    str r2, [r0, #0x20]
    mov r3, #0
    str r3, [r0, #4]
    str r3, [r0, #0xc]
    str r3, [r0, #0x14]
    str r3, [r0, #0x1c]
    neg r2, r1
    mov r3, #1
    lsl r3, r3, #12
    str r1, [r0, #0x18]
    str r2, [r0, #8]
    str r3, [r0, #0x10]
    bx lr
}
