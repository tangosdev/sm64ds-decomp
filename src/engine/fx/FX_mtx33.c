/* NitroSDK fx_mtx33.c: fixed-point 3x3 matrix construction, vector
 * multiplication and concatenation. The hand-written asm matrix constructors
 * precede the compiled C bodies in mwccarm's object. */
#include "types.h"
#include "math/Matrix.h"
#include "nitro/fx/fx_mtx33.h"

typedef long long s64;
typedef struct {
    int m[9];
} Mat9;

void MTX_Concat33(const int *A, const int *B, int *dst) {
    int tmp[9];
    int a0, a1, a2;
    int b0, b1, b2;
    int *d = dst;

    if (dst == B)
        d = tmp;

    a0 = A[0];
    a1 = A[1];
    a2 = A[2];
    d[0] = (((s64)a0 * B[0] + (s64)a1 * B[3] + (s64)a2 * B[6]) >> 12);
    d[1] = (((s64)a0 * B[1] + (s64)a1 * B[4] + (s64)a2 * B[7]) >> 12);
    b0 = B[2];
    b1 = B[5];
    b2 = B[8];
    d[2] = (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);

    a0 = A[3];
    a1 = A[4];
    a2 = A[5];
    d[5] = (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);
    d[4] = (((s64)a0 * B[1] + (s64)a1 * B[4] + (s64)a2 * B[7]) >> 12);
    b0 = B[0];
    b1 = B[3];
    b2 = B[6];
    d[3] = (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);

    a0 = A[6];
    a1 = A[7];
    a2 = A[8];
    d[6] = (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);
    d[7] = (((s64)a0 * B[1] + (s64)a1 * B[4] + (s64)a2 * B[7]) >> 12);
    d[8] = (((s64)a0 * B[2] + (s64)a1 * B[5] + (s64)a2 * B[8]) >> 12);

    if (d == tmp) {
        *(Mat9 *)dst = *(Mat9 *)tmp;
    }
}

/* dst = vec * m, the rotation alone (NitroSDK fx_mtx33.c). */
void MTX_MultVec33(const VecFx32 *vec, const MtxFx33 *m, VecFx32 *dst) {
    fx32 x = vec->x;
    fx32 y = vec->y;
    fx32 z = vec->z;

    dst->x =
        (fx32)(((fx64)x * m->m[0][0] + (fx64)y * m->m[1][0] + (fx64)z * m->m[2][0]) >> FX32_SHIFT);
    dst->y =
        (fx32)(((fx64)x * m->m[0][1] + (fx64)y * m->m[1][1] + (fx64)z * m->m[2][1]) >> FX32_SHIFT);
    dst->z =
        (fx32)(((fx64)x * m->m[0][2] + (fx64)y * m->m[1][2] + (fx64)z * m->m[2][2]) >> FX32_SHIFT);
}

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.

// Sets m to the identity matrix:
//   1 0 0
//   0 1 0
//   0 0 1
// 1 is written as 0x1000 (fixed point; the DS has no FPU).
// m arrives in r0. "stmia r0!, {r2, r3}" stores r2 and r3 into the next
// two entries of m and moves r0 past them, so m is written in pairs.
// Nine entries don't split into pairs, so the last 1 (m[8]) goes first.
asm void MTX_Identity33_(Matrix3x3 *m)
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

// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive: block copy,
// CP15, IRQ/CPSR, svc, matrix/math, context-switch). No C to decompile to. Per asm policy.
// NitroSDK fx_mtx33.c: copies the 3x3 src into the upper-left of the 4x4 dst and
// fills the rest of dst from the identity.
asm void MTX_Copy33To44_(void *src, void *dst) {
    stmdb sp!, {r4}
    mov ip, #0
    ldmia r0!, {r2, r3, r4}
    stmia r1!, {r2, r3, r4, ip}
    ldmia r0!, {r2, r3, r4}
    stmia r1!, {r2, r3, r4, ip}
    ldmia r0!, {r2, r3, r4}
    stmia r1!, {r2, r3, r4, ip}
    mov r2, #0x1000
    str ip, [r1]
    str ip, [r1, #4]
    str ip, [r1, #8]
    str r2, [r1, #0xc]
    ldmia sp!, {r4}
    bx lr
}

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

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// MTX_RotY33_ @ 0x0205256c (arm9, Thumb): build 3x3 fx32 Y-rotation matrix
// from (sin, cos): {c, 0, -s, 0, 1, 0, s, 0, c} with 4096 = 1.0. Identified as
// the Nitro SDK's FX_mtx33.c MTX_RotY33_ from pret's FX_mtx33.c, whose body
// is the same instructions.

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

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// MTX_RotZ33_ @ 0x02052588 (arm9, Thumb): build 3x3 fx32 Z-rotation
// matrix from (sin, cos): {c, s, 0, -s, c, 0, 0, 0, 1} with 4096 = 1.0.

asm void MTX_RotZ33_(Matrix3x3 *m, int s, int c) {
    stmia r0!, {r2}
    mov r3, #0
    stmia r0!, {r1, r3}
    neg r1, r1
    stmia r0!, {r1, r2}
    mov r1, #1
    lsl r1, r1, #12
    str r3, [r0, #0]
    str r3, [r0, #4]
    str r3, [r0, #8]
    str r1, [r0, #0xc]
    bx lr
}

