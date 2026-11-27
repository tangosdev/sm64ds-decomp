/* NitroSDK fx_mtx43.c: fixed-point 4x3 affine matrix construction,
 * vector multiplication, concatenation and inversion. The hand-written asm
 * matrix constructors precede the compiled C bodies in mwccarm's object. */
#include "types.h"
#include "common.h"
#include "math/Matrix.h"
#include "nitro/fx/fx_mtx43.h"
#include "nitro/fx/fx_cp.h"
#include "nitro/mi/memory.h"

int MTX_Inverse43(const MtxFx43 *src, MtxFx43 *dst) {
    MtxFx43 buf;
    MtxFx43 *out;
    int p, q, r;
    int det;
    int recip;

    out = (src == dst) ? &buf : dst;

    p = (int)(((s64)src->m[1][1] * src->m[2][2] - (s64)src->m[1][2] * src->m[2][1] + 0x800) >> 12);
    q = (int)(((s64)src->m[1][0] * src->m[2][2] - (s64)src->m[1][2] * src->m[2][0] + 0x800) >> 12);
    r = (int)(((s64)src->m[1][0] * src->m[2][1] - (s64)src->m[1][1] * src->m[2][0] + 0x800) >> 12);

    det = (int)(((s64)src->m[0][0] * p - (s64)src->m[0][1] * q + (s64)src->m[0][2] * r + 0x800) >>
                12);
    if (det == 0) {
        return -1;
    }

    FX_InvAsync(det);

    {
        int x1 = (int)(((s64)src->m[0][1] * src->m[2][2] - (s64)src->m[2][1] * src->m[0][2]) >> 12);
        int x2 = (int)(((s64)src->m[0][1] * src->m[1][2] - (s64)src->m[1][1] * src->m[0][2]) >> 12);
        int x3 = (int)(((s64)src->m[0][0] * src->m[2][2] - (s64)src->m[2][0] * src->m[0][2]) >> 12);
        int x4 = (int)(((s64)src->m[0][0] * src->m[1][2] - (s64)src->m[1][0] * src->m[0][2]) >> 12);

        recip = FX_GetDivResult();

        out->m[0][0] = (int)(((s64)recip * p) >> 12);
        out->m[0][1] = -(int)(((s64)recip * x1) >> 12);
        out->m[0][2] = (int)(((s64)recip * x2) >> 12);
        out->m[1][0] = -(int)(((s64)recip * q) >> 12);
        out->m[1][1] = (int)(((s64)recip * x3) >> 12);
        out->m[1][2] = -(int)(((s64)recip * x4) >> 12);
        out->m[2][0] = (int)(((s64)recip * r) >> 12);
    }

    {
        int x5 = (int)(((s64)src->m[0][0] * src->m[2][1] - (s64)src->m[2][0] * src->m[0][1]) >> 12);
        out->m[2][1] = -(int)(((s64)recip * x5) >> 12);
    }
    {
        int x6 = (int)(((s64)src->m[0][0] * src->m[1][1] - (s64)src->m[1][0] * src->m[0][1]) >> 12);
        out->m[2][2] = (int)(((s64)recip * x6) >> 12);
    }

    out->m[3][0] = -(int)((((s64)out->m[0][0] * src->m[3][0]) + ((s64)out->m[1][0] * src->m[3][1]) +
                           ((s64)out->m[2][0] * src->m[3][2])) >>
                          12);
    out->m[3][1] = -(int)((((s64)out->m[0][1] * src->m[3][0]) + ((s64)out->m[1][1] * src->m[3][1]) +
                           ((s64)out->m[2][1] * src->m[3][2])) >>
                          12);
    out->m[3][2] = -(int)((((s64)out->m[0][2] * src->m[3][0]) + ((s64)out->m[1][2] * src->m[3][1]) +
                           ((s64)out->m[2][2] * src->m[3][2])) >>
                          12);

    if (out == &buf) {
        MI_Copy48B(&buf, dst);
    }

    return 0;
}

typedef long long s64;

void MTX_Concat43(const Matrix4x3 *A, const Matrix4x3 *B, Matrix4x3 *dst) {
    Matrix4x3 tmp;
    int a0, a1, a2;
    int b0, b1, b2;
    Matrix4x3 *d = dst;

    if (dst == B)
        d = &tmp;

    a0 = A->r.m[0][0];
    a1 = A->r.m[0][1];
    a2 = A->r.m[0][2];
    d->r.m[0][0] =
        (((s64)a0 * B->r.m[0][0] + (s64)a1 * B->r.m[1][0] + (s64)a2 * B->r.m[2][0]) >> 12);
    d->r.m[0][1] =
        (((s64)a0 * B->r.m[0][1] + (s64)a1 * B->r.m[1][1] + (s64)a2 * B->r.m[2][1]) >> 12);
    b0 = B->r.m[0][2];
    b1 = B->r.m[1][2];
    b2 = B->r.m[2][2];
    d->r.m[0][2] = (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);

    a0 = A->r.m[1][0];
    a1 = A->r.m[1][1];
    a2 = A->r.m[1][2];
    d->r.m[1][2] = (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);
    d->r.m[1][1] =
        (((s64)a0 * B->r.m[0][1] + (s64)a1 * B->r.m[1][1] + (s64)a2 * B->r.m[2][1]) >> 12);
    b0 = B->r.m[0][0];
    b1 = B->r.m[1][0];
    b2 = B->r.m[2][0];
    d->r.m[1][0] = (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);

    a0 = A->r.m[2][0];
    a1 = A->r.m[2][1];
    a2 = A->r.m[2][2];
    d->r.m[2][0] = (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);
    d->r.m[2][1] =
        (((s64)a0 * B->r.m[0][1] + (s64)a1 * B->r.m[1][1] + (s64)a2 * B->r.m[2][1]) >> 12);
    b0 = B->r.m[0][2];
    b1 = B->r.m[1][2];
    b2 = B->r.m[2][2];
    d->r.m[2][2] = (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12);

    a0 = A->t.x;
    a1 = A->t.y;
    a2 = A->t.z;
    d->t.z = ((s64)B->t.z + (((s64)a0 * b0 + (s64)a1 * b1 + (s64)a2 * b2) >> 12));
    d->t.y = ((s64)B->t.y +
              (((s64)a0 * B->r.m[0][1] + (s64)a1 * B->r.m[1][1] + (s64)a2 * B->r.m[2][1]) >> 12));
    d->t.x = ((s64)B->t.x +
              (((s64)a0 * B->r.m[0][0] + (s64)a1 * B->r.m[1][0] + (s64)a2 * B->r.m[2][0]) >> 12));

    if (d == &tmp) {
        *dst = tmp;
    }
}

/* dst = vec * m, rotation then translation (NitroSDK fx_mtx43.c). */
void MTX_MultVec43(const VecFx32 *vec, const MtxFx43 *m, VecFx32 *dst) {
    fx32 x = vec->x;
    fx32 y = vec->y;
    fx32 z = vec->z;

    dst->x =
        (fx32)(((fx64)x * m->m[0][0] + (fx64)y * m->m[1][0] + (fx64)z * m->m[2][0]) >> FX32_SHIFT);
    dst->x += m->m[3][0];
    dst->y =
        (fx32)(((fx64)x * m->m[0][1] + (fx64)y * m->m[1][1] + (fx64)z * m->m[2][1]) >> FX32_SHIFT);
    dst->y += m->m[3][1];
    dst->z =
        (fx32)(((fx64)x * m->m[0][2] + (fx64)y * m->m[1][2] + (fx64)z * m->m[2][2]) >> FX32_SHIFT);
    dst->z += m->m[3][2];
}

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.

// Sets m to the identity matrix with no translation:
//   1 0 0
//   0 1 0
//   0 0 1
//   0 0 0   <- translation (x, y, z)
// 1 is written as 0x1000 (fixed point; the DS has no FPU).
// m arrives in r0. "stmia r0!, {r2, r3}" stores r2 and r3 into the next
// two entries of m and moves r0 past them, so m is written in pairs.
asm void MTX_Identity43_(Matrix4x3 *m)
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

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// MTX_Scale43_ @ 0x020527e8 (arm9, Thumb): build 4x3 fx32 scale matrix
// from (sx, sy, sz): rows {sx,0,0},{0,sy,0},{0,0,sz},{0,0,0}.

#pragma thumb on
asm void MTX_Scale43_(Matrix4x3 *m, int sx, int sy, int sz) {
    stmia r0!, {r1}
    mov r1, #0
    str r3, [r0, #0x1c]
    mov r3, #0
    stmia r0!, {r1, r3}
    stmia r0!, {r1, r2, r3}
    mov r2, #0
    stmia r0!, {r1, r3}
    add r0, #4
    stmia r0!, {r1, r2, r3}
    bx lr
}

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// MTX_RotX43_ @ 0x02052800 (arm9, Thumb): build 4x3 fx32 X-rotation matrix
// from (sin, cos): rows {1,0,0},{0,c,s},{0,-s,c},{0,0,0} with 4096 = 1.0.
union Matrix4x3;

asm void MTX_RotX43_(union Matrix4x3 *m, int s, int c) {
    str r1, [r0, #0x14]
    neg r1, r1
    str r1, [r0, #0x1c]
    mov r1, #1
    lsl r1, r1, #12
    stmia r0!, {r1}
    mov r3, #0
    mov r1, #0
    stmia r0!, {r1, r3}
    stmia r0!, {r1, r2}
    str r1, [r0, #4]
    add r0, #0xc
    stmia r0!, {r2, r3}
    stmia r0!, {r1, r3}
    bx lr
}

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// MTX_RotY43_ @ 0x02052820 (arm9, Thumb): build 4x3 fx32 Y-rotation matrix
// from (sin, cos): rows {c,0,-s},{0,1,0},{s,0,c},{0,0,0} with 4096 = 1.0.
union Matrix4x3;

asm void MTX_RotY43_(union Matrix4x3 *m, int s, int c) {
    str r1, [r0, #0x18]
    mov r3, #0
    stmia r0!, {r2, r3}
    neg r1, r1
    stmia r0!, {r1, r3}
    mov r1, #1
    lsl r1, r1, #12
    stmia r0!, {r1, r3}
    add r0, #4
    mov r1, #0
    stmia r0!, {r1, r2, r3}
    stmia r0!, {r1, r3}
    bx lr
}

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// MTX_RotZ43_ @ 0x0205283c (arm9, Thumb): build 4x3 fx32 Z-rotation matrix
// from (sin, cos): rows {c,s,0},{-s,c,0},{0,0,1},{0,0,0} with 4096 = 1.0.
union Matrix4x3;

asm void MTX_RotZ43_(union Matrix4x3 *m, int s, int c) {
    stmia r0!, {r2}
    mov r3, #0
    stmia r0!, {r1, r3}
    neg r1, r1
    stmia r0!, {r1, r2, r3}
    mov r1, #0
    mov r2, #0
    mov r3, #1
    lsl r3, r3, #12
    stmia r0!, {r1, r2, r3}
    mov r3, #0
    stmia r0!, {r1, r2, r3}
    bx lr
}

