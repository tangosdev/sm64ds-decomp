// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: compiler unsigned-divide runtime head (computed `add pc, pc, r2, lsl #2` dispatch).
// @ 0x0238D7A0 size 0x58 (runtime 0x03805638, wram)
/* Hand-asm: head of the unsigned-divide routine. Ends in `add pc, pc, r2, lsl #2`,
   a computed jump into the unrolled divide-step stream in fn_0238D7F8 — the
   PC-written dispatch is unreachable from C. No calls, no pool. */

asm void fn_0238D7A0(void)
{
    cmp r1, #0
    bxeq lr
    cmp r0, r1
    movlo r1, r0
    movlo r0, #0
    bxlo lr
    mov r2, #0x1C
    mov r3, r0, lsr #4
    cmp r1, r3, lsr #12
    suble r2, r2, #0x10
    movle r3, r3, lsr #16
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
}
