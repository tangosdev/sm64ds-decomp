// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
asm int func_0206da64(void) {
    mov r0, #1
    b lbl
    mov r0, #0
    b lbl
lbl:
    mrc p15, 0, r1, c1, c0, 0
    and r2, r1, #1
    tst r0, r0
    biceq r1, r1, #1
    orrne r1, r1, #1
    mcr p15, 0, r1, c1, c0, 0
    mov r0, r2
    bx lr
}
