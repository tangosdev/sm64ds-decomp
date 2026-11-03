// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
#include "math/Matrix.h"

// Loads the identity: twelve 20.12 fixed-point words, 1.0 = 0x1000,
//   1 0 0 / 0 1 0 / 0 0 1 / 0 0 0 (translation).
// Three columns per row puts the 1s on every fourth word, so the matrix is
// the pair (1,0),(0,0) three times over: six two-word stmia, no loop.
// {r1,r3} is the zero pair because a register list can't name r3 twice.
asm void Matrix4x3_LoadIdentity(Matrix4x3 *m)
{
    mov r2, #0x1000
    mov r3, #0
    stmia r0!, {r2, r3}
    mov r1, #0
    stmia r0!, {r1, r3}
    stmia r0!, {r2, r3}
    stmia r0!, {r1, r3}
    stmia r0!, {r2, r3}
    stmia r0!, {r1, r3}
    bx lr
}
