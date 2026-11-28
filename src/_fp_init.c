extern unsigned int *func_0207322c(void);
extern void *func_02073238(void);


asm void _fp_init(void)
{
    stmdb sp!, {lr}
    bl func_0207322c
    mov r1, #0
    str r1, [r0]
    bl func_02073238
    mov r1, #0
    str r1, [r0, #4]
    str r1, [r0, #8]
    str r1, [r0, #0xc]
    str r1, [r0, #0x10]
    str r1, [r0, #0x14]
    ldmia sp!, {lr}
    bx lr
}

asm unsigned int __ieee_status(unsigned int mask, unsigned int flags)
{
    stmdb sp!, {r0, r1, r2, r12, lr}
    bl func_0207322c
    mov r3, r0
    ldmia sp!, {r0, r1, r2, r12, lr}
    ldr r2, [r3]
    bic r12, r2, r0
    eor r12, r12, r1
    str r12, [r3]
    mov r0, r2
    bx lr
}

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
