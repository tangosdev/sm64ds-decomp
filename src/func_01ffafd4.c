extern void func_0207322c(void);
extern void func_02073238(void);

asm void func_01ffafd4(void) {
    stmdb   sp!, {lr}
    bl      func_0207322c
    mov     r1, #0
    str     r1, [r0]
    bl      func_02073238
    mov     r1, #0
    str     r1, [r0, #4]
    str     r1, [r0, #8]
    str     r1, [r0, #0xc]
    str     r1, [r0, #0x10]
    str     r1, [r0, #0x14]
    ldmia   sp!, {lr}
    bx      lr
}
