// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only runtime primitive).
extern void func_0207322c(void);
extern void func_0206dd7c(void);

asm void func_01ff859c(void) {
    tst     r1, #0x80000000
    bne     _01ff8690
    ldr     r2, =0x43e
    subs    r2, r2, r1, lsr #20
    blt     _01ff86a8
    cmp     r2, #0x40
    bge     _01ff8658
    mov     ip, r1, lsl #11
    orr     ip, ip, #0x80000000
    orr     ip, ip, r0, lsr #21
    cmp     r2, #0x20
    ble     _01ff8614
    sub     r2, r2, #0x20
    mov     r3, r0, lsl #11
    mov     r1, #0
    mov     r0, ip, lsr r2
    rsb     r2, r2, #0x20
    orrs    ip, r3, ip, lsl r2
    bxeq    lr
    stmdb   sp!, {r0, r1, r3, ip, lr}
    bl      func_0207322c
    mov     r2, r0
    ldmia   sp!, {r0, r1, r3, ip, lr}
    ldr     r3, [r2]
    tst     r3, #0x1000
    bne     _01ff86e8
    orr     r3, r3, #0x10
    str     r3, [r2]
    bx      lr
_01ff8614:
    mov     r3, r0, lsl #11
    mov     r1, ip, lsr r2
    mov     r0, r3, lsr r2
    rsb     r2, r2, #0x20
    orr     r0, r0, ip, lsl r2
    movs    ip, r3, lsl r2
    bxeq    lr
    stmdb   sp!, {r0, r1, r3, ip, lr}
    bl      func_0207322c
    mov     r2, r0
    ldmia   sp!, {r0, r1, r3, ip, lr}
    ldr     r3, [r2]
    tst     r3, #0x1000
    bne     _01ff86e8
    orr     r3, r3, #0x10
    str     r3, [r2]
    bx      lr
_01ff8658:
    orrs    r1, r1, r0
    bxeq    lr
    mov     r1, #0
    mov     r0, #0
    stmdb   sp!, {r0, r1, r3, ip, lr}
    bl      func_0207322c
    mov     r2, r0
    ldmia   sp!, {r0, r1, r3, ip, lr}
    ldr     r3, [r2]
    tst     r3, #0x1000
    bne     _01ff86e8
    orr     r3, r3, #0x10
    str     r3, [r2]
    bx      lr
_01ff8690:
    cmn     r1, #0x100000
    cmpeq   r0, #0
    bhi     _01ff86a8
    mov     r1, #0
    mov     r0, #0
    bx      lr
_01ff86a8:
    stmdb   sp!, {r0, r1, r3, ip, lr}
    bl      func_0207322c
    mov     r2, r0
    ldmia   sp!, {r0, r1, r3, ip, lr}
    ldr     r3, [r2]
    tst     r3, #0x100
    bne     _01ff86d4
    orr     r3, r3, #1
    str     r3, [r2]
    mvn     r1, #0
    mvn     r0, #0
    bx      lr
_01ff86d4:
    ldr     r2, =0x4c000f8
    stmdb   sp!, {lr}
    bl      func_0206dd7c
    ldmia   sp!, {lr}
    bx      lr
_01ff86e8:
    ldr     r2, =0x40c000f8
    stmdb   sp!, {lr}
    bl      func_0206dd7c
    ldmia   sp!, {lr}
    bx      lr
}
