// HAND-ASM PRIMITIVE: byte-faithful asm-block match.
extern void func_0207322c(void);
extern void func_0206ddcc(void);

asm void func_01ffa344(void) {
    bic     r1, r0, #0x80000000
    mov     r2, #0x9e
    subs    r2, r2, r1, lsr #23
    ble     _01ffa3d8
    cmp     r2, #0x20
    bge     _01ffa3a4
    mov     r1, r1, lsl #8
    orr     r1, r1, #0x80000000
    cmp     r0, #0
    mov     r0, r1, lsr r2
    rsbmi   r0, r0, #0
    rsb     r2, r2, #0x20
    movs    r1, r1, lsl r2
    bxeq    lr
    stmdb   sp!, {r0, r1, r3, ip, lr}
    bl      func_0207322c
    mov     r2, r0
    ldmia   sp!, {r0, r1, r3, ip, lr}
    ldr     r3, [r2]
    tst     r3, #0x1000
    bne     _01ffa424
    orr     r3, r3, #0x10
    str     r3, [r2]
    bx      lr
_01ffa3a4:
    movs    r0, r1
    bxeq    lr
    mov     r0, #0
    stmdb   sp!, {r0, r2, r3, ip, lr}
    bl      func_0207322c
    mov     r1, r0
    ldmia   sp!, {r0, r2, r3, ip, lr}
    ldr     r3, [r1]
    tst     r3, #0x1000
    bne     _01ffa424
    orr     r3, r3, #0x10
    str     r3, [r1]
    bx      lr
_01ffa3d8:
    cmp     r0, #0xcf000000
    beq     _01ffa404
    stmdb   sp!, {r0, r1, r3, ip, lr}
    bl      func_0207322c
    mov     r2, r0
    ldmia   sp!, {r0, r1, r3, ip, lr}
    ldr     r3, [r2]
    tst     r3, #0x100
    bne     _01ffa410
    orr     r3, r3, #1
    str     r3, [r2]
_01ffa404:
    mvn     r0, r0, asr #31
    add     r0, r0, #0x80000000
    bx      lr
_01ffa410:
    ldr     r1, =0x04c00048
    stmdb   sp!, {lr}
    bl      func_0206ddcc
    ldmia   sp!, {lr}
    bx      lr
_01ffa424:
    ldr     r1, =0x40c00048
    stmdb   sp!, {lr}
    bl      func_0206ddcc
    ldmia   sp!, {lr}
    bx      lr
}
