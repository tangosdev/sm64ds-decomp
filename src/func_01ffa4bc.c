// HAND-ASM PRIMITIVE: byte-faithful asm-block match.
extern void func_0207322c(void);
extern void func_0206ddcc(void);

asm void func_01ffa4bc(void) {
    ands    r2, r0, #0x80000000
    rsbmi   r0, r0, #0
    cmp     r0, #0
    bxeq    lr
    clz     r3, r0
    movs    r0, r0, lsl r3
    rsb     r3, r3, #0x9e
    ands    r1, r0, #0xff
    add     r0, r0, r0
    orr     r0, r2, r0, lsr #9
    orr     r0, r0, r3, lsl #23
    bxeq    lr
    stmdb   sp!, {r0, r1, r2, r3, lr}
    bl      func_0207322c
    mov     r12, r0
    ldmia   sp!, {r0, r1, r2, r3, lr}
    ldr     r3, [r12]
    tst     r3, #0x1000
    bne     _01ffa56c
    orr     r3, r3, #0x10
    str     r3, [r12]
    tst     r3, #0x800000
    bne     _01ffa540
    tst     r3, #0x400000
    bne     _01ffa54c
    tst     r1, #0x80
    bxeq    lr
    ands    r3, r1, #0x7f
    andeqs  r3, r0, #1
    addne   r0, r0, #1
    bx      lr
_01ffa538:
    add     r0, r0, #1
    bx      lr
_01ffa540:
    tst     r3, #0x400000
    addeq   r0, r0, r2, lsr #31
    bx      lr
_01ffa54c:
    cmp     r2, #0
    addpl   r0, r0, #1
    bx      lr
_01ffa558:
    stmdb   sp!, {lr}
    ldr     r1, =0x40000308
    bl      func_0206ddcc
    ldmia   sp!, {lr}
    bx      lr
_01ffa56c:
    stmdb   sp!, {lr}
    ldr     r1, =0x40000208
    bl      func_0206ddcc
    ldmia   sp!, {lr}
    bx      lr
}
