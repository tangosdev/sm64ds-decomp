// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// MTX_RotX43_ @ 0x02052800 (arm9, Thumb): build 4x3 fx32 X-rotation matrix
// from (sin, cos): rows {1,0,0},{0,c,s},{0,-s,c},{0,0,0} with 4096 = 1.0.
struct Matrix4x3;

#pragma thumb on
#ifndef SM64DS_PLATFORM_PC
asm void MTX_RotX43_(struct Matrix4x3 *m, int s, int c) {
    str r1, [r0, #20]
    neg r1, r1
    str r1, [r0, #28]
    mov r1, #1
    lsl r1, r1, #12
    stmia r0!, {r1}
    mov r3, #0
    mov r1, #0
    stmia r0!, {r1, r3}
    stmia r0!, {r1, r2}
    str r1, [r0, #4]
    add r0, #12
    stmia r0!, {r2, r3}
    stmia r0!, {r1, r3}
    bx lr
}
#else
void MTX_RotX43_(struct Matrix4x3 *m, int s, int c)
{
    int *p = (int *)m;
    p[0] = 0x1000;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    p[4] = c;
    p[5] = s;
    p[6] = 0;
    p[7] = -s;
    p[8] = c;
    p[9] = 0;
    p[10] = 0;
    p[11] = 0;
}
#endif
