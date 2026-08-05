// HAND-ASM PRIMITIVE: byte-faithful asm-block match.
extern void func_0207322c(void);
extern void _01ffa558(void);
extern void _01ffa538(void);

asm void func_01ffa440(void) {
    cmp     r0, #0
    bxeq    lr
    mov     r3, #0x9e
    bmi     _01ffa45c
    clz     r12, r0
    movs    r0, r0, lsl r12
    sub     r3, r3, r12
_01ffa45c:
    ands    r2, r0, #0xff
    add     r0, r0, r0
    mov     r0, r0, lsr #9
    orr     r0, r0, r3, lsl #23
    bxeq    lr
    stmdb   sp!, {r0, r2, r3, r12, lr}
    bl      func_0207322c
    mov     r1, r0
    ldmia   sp!, {r0, r2, r3, r12, lr}
    ldr     r3, [r1]
    tst     r3, #0x1000
    bne     _01ffa558
    orr     r3, r3, #0x10
    str     r3, [r1]
    tst     r3, #0x800000
    bxne    lr
    tst     r3, #0x400000
    bne     _01ffa538
    tst     r2, #0x80
    bxeq    lr
    ands    r1, r2, #0x7f
    andeqs  r1, r0, #1
    addne   r0, r0, #1
    bx      lr
}
