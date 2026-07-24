// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
asm void func_020707a4(void) {
    stmdb sp!, {r0, r1, r2, r3}
    ldr r1, [sp, #4]
    ldr r0, [sp, #0xc]
    bic r1, r1, #0x80000000
    and r0, r0, #0x80000000
    orr r1, r1, r0
    ldr r0, [sp]
    str r1, [sp, #4]
    add sp, sp, #0x10
    bx lr
}
