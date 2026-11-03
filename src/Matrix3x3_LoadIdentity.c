// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
#include "math/Matrix.h"

// Loads the identity: nine 20.12 fixed-point words, 1.0 = 0x1000.
// Nine is one too many for whole pairs, so the last 1.0 (m[8], +0x20) is
// stored first while r0 still points at the start; four stmia pairs then
// write m[0..7], (1,0),(0,0) twice.
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
