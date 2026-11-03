// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
#include "math/Matrix.h"

// Sets m to the identity matrix:
//   1 0 0
//   0 1 0
//   0 0 1
// 1 is written as 0x1000 (fixed point; the DS has no FPU).
// m arrives in r0. "stmia r0!, {r2, r3}" stores r2 and r3 into the next
// two entries of m and moves r0 past them, so m is written in pairs.
// Nine entries don't split into pairs, so the last 1 (m[8]) goes first.
asm void Matrix3x3_LoadIdentity(Matrix3x3 *m)
{
    mov r2, #0x1000
    str r2, [r0, #0x20]
    mov r3, #0
    stmia r0!, {r2, r3}
    mov r1, #0
    stmia r0!, {r1, r3}
    stmia r0!, {r2, r3}
    stmia r0!, {r1, r3}
    bx lr
}
