// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// MTX_RotZ33_ @ 0x02052588 (arm9, Thumb): build 3x3 fx32 Z-rotation
// matrix from (sin, cos): {c, s, 0, -s, c, 0, 0, 0, 1} with 4096 = 1.0.
#pragma thumb on
#ifndef SM64DS_PLATFORM_PC
asm void MTX_RotZ33_(int *m, int s, int c) {
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
    str r1, [r0, #12]
    bx lr
}
#else
void MTX_RotZ33_(int *m, int s, int c)
{
    m[0] = c;
    m[1] = s;
    m[2] = 0;
    m[3] = -s;
    m[4] = c;
    m[5] = 0;
    m[6] = 0;
    m[7] = 0;
    m[8] = 0x1000;
}
#endif
