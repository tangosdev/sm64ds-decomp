// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// func_02052820 @ 0x02052820 (arm9, Thumb): build 4x3 fx32 Y-rotation matrix
// from (sin, cos): rows {c,0,-s},{0,1,0},{s,0,c},{0,0,0} with 4096 = 1.0.
#pragma thumb on
asm void func_02052820(int *m, int s, int c) {
    str r1, [r0, #24]
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
