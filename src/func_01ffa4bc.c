// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// func_01ffa4bc at 0x01ffa4bc (ITCM) -- 32-bit integer to float conversion, the inverse
// of func_01ffa344 above it and part of the same soft-float runtime block. Assembly in
// the original.
//
// Takes the sign off into r2, normalises with `clz`, and builds the result from the
// 0x9e exponent bias directly. If the eight bits shifted out of the bottom are non-zero
// the value is inexact, so it reads the FP status word through func_0207322c, sets the
// inexact flag and then picks a rounding direction from the mode bits: round-to-nearest
// (ties to even, hence the `ands r3, r1, #0x7f` / `andeqs r3, r0, #1` pair), toward
// +infinity, or toward zero. An enabled inexact trap exits through func_0206ddcc with
// the packed exception code 0x40000208, and 0x40000308 is the same for the round-up path.
//
// The argument registers are pushed across the call as they are in every other routine in
// this block, which is the tell that these are one hand-written assembly file.

extern int func_0207322c(void);
extern void func_0206ddcc(void);

asm float func_01ffa4bc(int value)
{
    ands r2, r0, #0x80000000
    rsbmi r0, r0, #0
    cmp r0, #0
    bxeq lr
    clz r3, r0
    movs r0, r0, lsl r3
    rsb r3, r3, #0x9e
    ands r1, r0, #0xff
    add r0, r0, r0
    orr r0, r2, r0, lsr #9
    orr r0, r0, r3, lsl #23
    bxeq lr
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov r12, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [r12]
    tst r3, #0x1000
    bne _L0b0
    orr r3, r3, #0x10
    str r3, [r12]
    tst r3, #0x800000
    bne _L084
    tst r3, #0x400000
    bne _L090
    tst r1, #0x80
    bxeq lr
    ands r3, r1, #0x7f
    andeqs r3, r0, #1
    addne r0, r0, #1
    bx lr
    add r0, r0, #1
    bx lr
_L084:
    tst r3, #0x400000
    addeq r0, r0, r2, lsr #31
    bx lr
_L090:
    cmp r2, #0
    addpl r0, r0, #1
    bx lr
    stmdb sp!, {lr}
    ldr r1, =0x40000308
    bl func_0206ddcc
    ldmia sp!, {lr}
    bx lr
_L0b0:
    stmdb sp!, {lr}
    ldr r1, =0x40000208
    bl func_0206ddcc
    ldmia sp!, {lr}
    bx lr
}
