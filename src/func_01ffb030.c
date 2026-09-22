// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// func_01ffb030 at 0x01ffb030 (ITCM) -- the packed-field form of func_01ffb008 next door,
// for the floating-point runtime's two five-bit fields (the sticky exception flags in
// bits 0-4 and their enable mask in bits 16-20).
//
// Both arguments arrive with the second field at bit 16 and are repacked to bit 8 before
// the shared read-modify-write; the old word comes back repacked the other way, bits 0-4
// and 8-12 spread back out to 0-4 and 16-20, with bit 30 set as the "this is a status
// word" tag the caller checks. The middle eight instructions are byte-identical to
// func_01ffb008, which is what makes them one hand-written routine with two wrappers
// rather than two compiled functions.
//
// Assembly in the original for the same reason as func_01ffb008: the arguments are kept
// across the `bl` by pushing r0-r2 and ip, a register set no compiler-generated prologue
// saves.

extern int func_0207322c(void);

asm unsigned int func_01ffb030(unsigned int mask, unsigned int value)
{
    and r2, r0, #0x1f
    and r3, r0, #0x1f0000
    orr r0, r2, r3, lsr #8
    and r2, r1, #0x1f
    and r3, r1, #0x1f0000
    orr r1, r2, r3, lsr #8
    stmdb sp!, {r0, r1, r2, r12, lr}
    bl func_0207322c
    mov r3, r0
    ldmia sp!, {r0, r1, r2, r12, lr}
    ldr r2, [r3]
    bic r12, r2, r0
    eor r12, r12, r1
    str r12, [r3]
    and r0, r2, #0x1f
    and r2, r2, #0x1f00
    orr r0, r0, r2, lsl #8
    orr r0, r0, #0x40000000
    bx lr
}
