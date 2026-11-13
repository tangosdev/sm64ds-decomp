// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// __fp_status at 0x01ffb030 (ITCM) -- the ARM runtime's older interface to the
// floating-point status word: the same read-modify-write as __ieee_status (0x01ffb008, next
// door), wrapped in a repack between the two status-word layouts. It repeats that code
// rather than calling it.
//
// Named from ARM's own specification. ARM DUI0475 5.3: `unsigned int __fp_status(unsigned
// int mask, unsigned int flags)` "is the same as __ieee_status() but it uses an older style
// of status word layout". In that layout bits 0-4 are the sticky exception flags, bits
// 16-20 the exception masks, and bits 24-31 a read-only system ID "set to 0x40 for software
// floating-point". __ieee_status keeps the same flags in bits 0-4 and its masks in bits
// 8-12. This body is exactly that conversion:
//
//   - both arguments keep bits 0-4 and move bits 16-20 down to 8-12;
//   - the shared read-modify-write follows, its middle eight instructions byte-identical to
//     __ieee_status;
//   - the old word comes back with bits 8-12 moved up to 16-20 and 0x40000000 set, the
//     software-floating-point system ID.
//
// Assembly in the original for the same reason as __ieee_status: the arguments are kept
// across the `bl` by pushing r0-r2 and ip, a register set no compiler-generated prologue
// saves.

extern unsigned int *func_0207322c(void);

asm unsigned int __fp_status(unsigned int mask, unsigned int flags)
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
