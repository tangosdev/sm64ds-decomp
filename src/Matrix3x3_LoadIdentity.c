// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
#include "math/Matrix.h"

// Sets m to the identity matrix:
//   1 0 0
//   0 1 0
//   0 0 1
// 1 is written as 0x1000: the DS has no floating point, so these are
// fixed-point numbers with 12 fraction bits. Each stmia stores two
// registers at m (r0) and moves it past them, two entries at a time.
asm void Matrix3x3_LoadIdentity(Matrix3x3 *m)
{
    mov r2, #0x1000         // r2 = 1
    str r2, [r0, #0x20]     // m[8] = 1: nine entries don't pair up, so do this one first
    mov r3, #0              // r3 = 0
    stmia r0!, {r2, r3}     // m[0] = 1, m[1] = 0
    mov r1, #0              // r1 = 0
    stmia r0!, {r1, r3}     // m[2] = 0, m[3] = 0
    stmia r0!, {r2, r3}     // m[4] = 1, m[5] = 0
    stmia r0!, {r1, r3}     // m[6] = 0, m[7] = 0
    bx lr
}
