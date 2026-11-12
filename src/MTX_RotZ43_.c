// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// MTX_RotZ43_ @ 0x0205283c (arm9, Thumb): build 4x3 fx32 Z-rotation matrix
// from (sin, cos): rows {c,s,0},{-s,c,0},{0,0,1},{0,0,0} with 4096 = 1.0.
struct Matrix4x3;

#pragma thumb on
#ifndef SM64DS_PLATFORM_PC
asm void MTX_RotZ43_(struct Matrix4x3 *m, int s, int c) {
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
#else
void MTX_RotZ43_(struct Matrix4x3 *m, int s, int c)
{
    int *p = (int *)m;
    p[0] = c;
    p[1] = s;
    p[2] = 0;
    p[3] = -s;
    p[4] = c;
    p[5] = 0;
    p[6] = 0;
    p[7] = 0;
    p[8] = 0x1000;
    p[9] = 0;
    p[10] = 0;
    p[11] = 0;
}
#endif
