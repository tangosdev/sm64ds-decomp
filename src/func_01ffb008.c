extern void func_0207322c(void);

asm void func_01ffb008(void) {
    stmdb   sp!, {r0, r1, r2, ip, lr}
    bl      func_0207322c
    mov     r3, r0
    ldmia   sp!, {r0, r1, r2, ip, lr}
    ldr     r2, [r3]
    bic     ip, r2, r0
    eor     ip, ip, r1
    str     ip, [r3]
    mov     r0, r2
    bx      lr
}
