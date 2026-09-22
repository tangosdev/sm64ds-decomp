// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// _s32_div_f at 0x01ffabe4 (ITCM), also known as __aeabi_idiv -- the signed 32-bit
// integer divide the compiler calls for `/` and `%` on ints. Toolchain runtime, shipped
// as assembly; there is no C for it anywhere in the game's sources.
//
// It folds both signs into ip up front (bit 31 = quotient sign, bit 0 = remainder sign),
// takes absolute values, then runs the same unsigned kernel as _u32_div_f next door --
// three `lsr` compares to find the leading-zero gap, a computed `add pc, pc, r2, lsl #2`
// into an unrolled shift-subtract block, and the remainder left in r1 -- before negating
// the two results back. Two return values in r0 and r1 is outside the C ABI, which is
// part of why this is a runtime primitive and not a compiled function.
//
// Self-contained: no pool words and no branches outside its own 0x20c bytes. _ll_sdiv
// two functions up tail-calls into it for the 32-bit-operands fast path.

asm int _s32_div_f(int num, int den)
{
    eor r12, r0, r1
    and r12, r12, #0x80000000
    cmp r0, #0
    rsblt r0, r0, #0
    addlt r12, r12, #1
    cmp r1, #0
    rsblt r1, r1, #0
    beq _L1f8
    cmp r0, r1
    movlo r1, r0
    movlo r0, #0
    blo _L1f8
    mov r2, #0x1c
    mov r3, r0, lsr #4
    cmp r1, r3, lsr #12
    suble r2, r2, #0x10
    movle r3, r3, lsr #0x10
    cmp r1, r3, lsr #4
    suble r2, r2, #8
    movle r3, r3, lsr #8
    cmp r1, r3
    suble r2, r2, #4
    movle r3, r3, lsr #4
    mov r0, r0, lsl r2
    rsb r1, r1, #0
    adds r0, r0, r0
    add r2, r2, r2, lsl #1
    add pc, pc, r2, lsl #2
    mov r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    adcs r3, r1, r3, lsl #1
    sublo r3, r3, r1
    adcs r0, r0, r0
    mov r1, r3
_L1f8:
    ands r3, r12, #0x80000000
    rsbne r0, r0, #0
    ands r3, r12, #1
    rsbne r1, r1, #0
    bx lr
}
