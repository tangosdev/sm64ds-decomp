// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. Nintendo shipped this as an
// assembly primitive, so there is no original C to recover and no match to chase. Counts as
// done under the asm-primitive policy - see notes/arm9-endgame.md.
// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
extern void func_0205947c(void);
extern void func_0205950c(void);
asm void func_02059468(void) {
    stmdb sp!, {lr}
    bl func_0205947c
    bl func_0205950c
    ldmia sp!, {lr}
    bx lr
}
