// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// func_01ff859c at 0x01ff859c (ITCM) -- double to 64-bit integer conversion, sitting
// directly between _dadd (0x01ff8000) and _dmul (0x01ff8708) in the soft-float runtime
// block. Assembly in the original.
//
// 0x43e is 0x3ff + 63: the biased exponent of a double whose value is exactly 2^63, so
// `0x43e - (bits >> 20)` is the right shift that lands the 53-bit mantissa in a 64-bit
// integer. Above 0x40 the result is zero, below zero it overflows, and the two halves are
// produced separately because the shift can exceed 32 -- the result comes back in r0:r1.
//
// Anything shifted out makes the conversion inexact, which is handled the same way as in
// the rest of the block: read the FP status word through func_0207322c, set the inexact
// flag, and check the enable bits, with the trapping and out-of-range paths leaving
// through func_0206dd7c carrying a packed exception code (0x04c000f8 / 0x40c000f8).

extern int func_0207322c(void);
extern void func_0206dd7c(void);

asm long long func_01ff859c(double x)
{
    tst r1, #0x80000000
    bne _L0f0
    ldr r2, =0x0000043e
    subs r2, r2, r1, lsr #20
    blt _L108
    cmp r2, #0x40
    bge _L0b8
    mov r12, r1, lsl #0xb
    orr r12, r12, #0x80000000
    orr r12, r12, r0, lsr #21
    cmp r2, #0x20
    ble _L074
    sub r2, r2, #0x20
    mov r3, r0, lsl #0xb
    mov r1, #0
    mov r0, r12, lsr r2
    rsb r2, r2, #0x20
    orrs r12, r3, r12, lsl r2
    bxeq lr
    stmdb sp!, {r0, r1, r3, r12, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, r12, lr}
    ldr r3, [r2]
    tst r3, #0x1000
    bne _L14c
    orr r3, r3, #0x10
    str r3, [r2]
    bx lr
_L074:
    mov r3, r0, lsl #0xb
    mov r1, r12, lsr r2
    mov r0, r3, lsr r2
    rsb r2, r2, #0x20
    orr r0, r0, r12, lsl r2
    movs r12, r3, lsl r2
    bxeq lr
    stmdb sp!, {r0, r1, r3, r12, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, r12, lr}
    ldr r3, [r2]
    tst r3, #0x1000
    bne _L14c
    orr r3, r3, #0x10
    str r3, [r2]
    bx lr
_L0b8:
    orrs r1, r1, r0
    bxeq lr
    mov r1, #0
    mov r0, #0
    stmdb sp!, {r0, r1, r3, r12, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, r12, lr}
    ldr r3, [r2]
    tst r3, #0x1000
    bne _L14c
    orr r3, r3, #0x10
    str r3, [r2]
    bx lr
_L0f0:
    cmn r1, #0x100000
    cmpeq r0, #0
    bhi _L108
    mov r1, #0
    mov r0, #0
    bx lr
_L108:
    stmdb sp!, {r0, r1, r3, r12, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, r12, lr}
    ldr r3, [r2]
    tst r3, #0x100
    bne _L138
    orr r3, r3, #1
    str r3, [r2]
    mvn r1, #0
    mvn r0, #0
    bx lr
_L138:
    ldr r2, =0x04c000f8
    stmdb sp!, {lr}
    bl func_0206dd7c
    ldmia sp!, {lr}
    bx lr
_L14c:
    ldr r2, =0x40c000f8
    stmdb sp!, {lr}
    bl func_0206dd7c
    ldmia sp!, {lr}
    bx lr
}
