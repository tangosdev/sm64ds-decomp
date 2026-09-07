// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// func_01ffa344 at 0x01ffa344 (ITCM) -- float to 32-bit integer conversion, part of the
// soft-float runtime block that fills 0x01ff8000-0x01ffb078. Assembly in the original.
//
// Unbiases the exponent against 0x9e (127 + 31), shifts the mantissa into place with the
// hidden bit restored, and negates on the sign. The inexact tail is what makes it a
// runtime primitive rather than something a compiler emits: when bits are shifted out it
// reads the FP status word through func_0207322c, sets the inexact flag, and consults the
// enable and rounding-mode bits before deciding whether to round the result up -- and the
// out-of-range and enabled-exception paths hand off to func_0206ddcc with a packed
// exception code (0x04c00048 / 0x40c00048) in r1.
//
// It keeps live values across those calls by pushing the argument registers themselves
// (`stmdb sp!, {r0, r1, r3, ip, lr}`), a register set no compiler prologue saves.

extern int func_0207322c(void);
extern void func_0206ddcc(void);

asm int func_01ffa344(int bits)
{
    bic r1, r0, #0x80000000
    mov r2, #0x9e
    subs r2, r2, r1, lsr #23
    ble _L094
    cmp r2, #0x20
    bge _L060
    mov r1, r1, lsl #8
    orr r1, r1, #0x80000000
    cmp r0, #0
    mov r0, r1, lsr r2
    rsbmi r0, r0, #0
    rsb r2, r2, #0x20
    movs r1, r1, lsl r2
    bxeq lr
    stmdb sp!, {r0, r1, r3, r12, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, r12, lr}
    ldr r3, [r2]
    tst r3, #0x1000
    bne _L0e0
    orr r3, r3, #0x10
    str r3, [r2]
    bx lr
_L060:
    movs r0, r1
    bxeq lr
    mov r0, #0
    stmdb sp!, {r0, r2, r3, r12, lr}
    bl func_0207322c
    mov r1, r0
    ldmia sp!, {r0, r2, r3, r12, lr}
    ldr r3, [r1]
    tst r3, #0x1000
    bne _L0e0
    orr r3, r3, #0x10
    str r3, [r1]
    bx lr
_L094:
    cmp r0, #0xcf000000
    beq _L0c0
    stmdb sp!, {r0, r1, r3, r12, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, r12, lr}
    ldr r3, [r2]
    tst r3, #0x100
    bne _L0cc
    orr r3, r3, #1
    str r3, [r2]
_L0c0:
    mvn r0, r0, asr #31
    add r0, r0, #0x80000000
    bx lr
_L0cc:
    ldr r1, =0x04c00048
    stmdb sp!, {lr}
    bl func_0206ddcc
    ldmia sp!, {lr}
    bx lr
_L0e0:
    ldr r1, =0x40c00048
    stmdb sp!, {lr}
    bl func_0206ddcc
    ldmia sp!, {lr}
    bx lr
}
