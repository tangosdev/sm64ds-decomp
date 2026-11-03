// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
#include "math/Matrix.h"

// Sets m to the identity matrix with no translation:
//   1 0 0
//   0 1 0
//   0 0 1
//   0 0 0   <- translation (x, y, z)
// (1 is written as 0x1000: the DS has no floating point, so these are
// fixed-point numbers with 12 fraction bits.)
//
// stmia stores two registers to memory and moves the pointer past them,
// so the matrix is filled two entries at a time. r2 holds 1 and r1/r3
// hold 0, so {r2, r3} writes "1, 0" and {r1, r3} writes "0, 0".
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
