// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only runtime primitive).
// Segment/autoload initializer: reads a {dest, end, source} table from a pooled base,
// block-copies words then bzeroes ranges, and chains to the next init routine by falling
// through (b func_020049ec). Uses r4-r7 with no prologue/epilogue saving them and no
// bx lr return -- no C compiler emits that, so there is no C to decompile it to. Per asm policy.
extern void func_020049ec(void);
asm void func_0200497c(void) {
    ldr     r0, =0x2004ad8
    ldr     r1, [r0]
    ldr     r2, [r0, #4]
    ldr     r3, [r0, #8]
outer:
    cmp     r1, r2
    beq     tail
    ldr     r4, [r1], #4
    ldr     r5, [r1], #4
    add     r6, r4, r5
in1:
    cmp     r4, r6
    ldrmi   r7, [r3], #4
    strmi   r7, [r4], #4
    bmi     in1
    ldr     r5, [r1], #4
    add     r6, r4, r5
    mov     r7, #0
in2:
    cmp     r4, r6
    strlo   r7, [r4], #4
    blo     in2
    beq     outer
tail:
    ldr     r4, [r0, #0xc]
    ldr     r6, [r0, #0x10]
    mov     r7, #0
in3:
    cmp     r4, r6
    strlo   r7, [r4], #4
    blo     in3
    b       func_020049ec
}
