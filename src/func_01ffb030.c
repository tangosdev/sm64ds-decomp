extern void func_0207322c(void);

asm void func_01ffb030(void) {
    and     r2, r0, #0x1f
    and     r3, r0, #0x1f0000
    orr     r0, r2, r3, lsr #8
    and     r2, r1, #0x1f
    and     r3, r1, #0x1f0000
    orr     r1, r2, r3, lsr #8
    stmdb   sp!, {r0, r1, r2, ip, lr}
    bl      func_0207322c
    mov     r3, r0
    ldmia   sp!, {r0, r1, r2, ip, lr}
    ldr     r2, [r3]
    bic     ip, r2, r0
    eor     ip, ip, r1
    str     ip, [r3]
    and     r0, r2, #0x1f
    and     r2, r2, #0x1f00
    orr     r0, r0, r2, lsl #8
    orr     r0, r0, #0x40000000
    bx      lr
}
