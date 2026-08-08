// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// _dmul at 0x01ff8708 (ITCM), also known as func_01ff8708 -- double-precision multiply.
// The compiler emits a call to this for every `*` on a double; it is the toolchain's own
// runtime, shipped as assembly, and there is no C source for it in the game.
//
// Operands arrive in r0:r1 and r2:r3 and the product comes back in r0:r1, which is
// already outside the C ABI for doubles. The kernel is a 53x53 -> 106 bit multiply built
// out of `umull`/`umlal` partial products with the carries chained by hand, then a
// normalise-and-round step; the special cases (zero, denormal, infinity, NaN) are
// straight-line tails rather than branches out of a compiled control-flow graph.
//
// Every exceptional path reads the FP status word through func_0207322c and, when the
// matching trap is enabled, leaves through func_0206dd7c or func_0206dcd4 with a packed
// exception code. Those eight `bl` sites and the five pool words are its only references
// outside its own 0x6f0 bytes.

extern int func_0207322c(void);
extern void func_0206dd7c(void);
extern void func_0206dcd4(void);

asm double _dmul(double a, double b)
{
    stmdb sp!, {r0, r1, r2, r3, r4, r5, r6, r7, lr}
    eor lr, r1, r3
    and lr, lr, #0x80000000
    mov r12, r1, lsr #0x14
    mov r1, r1, lsl #0xb
    orr r1, r1, r0, lsr #21
    mov r0, r0, lsl #0xb
    movs r6, r12, lsl #0x15
    cmnne r6, #0x200000
    beq _L1a8
    orr r1, r1, #0x80000000
    bic r12, r12, #0x800
    mov r4, r3, lsr #0x14
    mov r3, r3, lsl #0xb
    orr r3, r3, r2, lsr #21
    mov r2, r2, lsl #0xb
    movs r5, r4, lsl #0x15
    cmnne r5, #0x200000
    beq _L1f0
    orr r3, r3, #0x80000000
    bic r4, r4, #0x800
_L054:
    add r12, r4, r12
    umull r5, r4, r0, r2
    umull r7, r6, r0, r3
    adds r4, r7, r4
    adc r6, r6, #0
    umull r7, r0, r1, r2
    adds r4, r7, r4
    adcs r0, r0, r6
    umull r7, r2, r1, r3
    adc r1, r2, #0
    adds r0, r0, r7
    adc r1, r1, #0
    orrs r4, r4, r5
    orrne r0, r0, #1
    cmp r1, #0
    blt _L0a0
    sub r12, r12, #1
    adds r0, r0, r0
    adc r1, r1, r1
_L0a0:
    add r12, r12, #2
    subs r12, r12, #0x400
    bmi _L2dc
    beq _L2dc
    mov r6, r12, lsl #0x14
    cmn r6, #0x100000
    bmi _L5a0
    movs r2, r0, lsl #0x15
    mov r0, r0, lsr #0xb
    orr r0, r0, r1, lsl #21
    add r1, r1, r1
    orr r1, lr, r1, lsr #12
    orr r1, r1, r12, lsl #20
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, r5, r6, r7, lr}
    bxeq lr
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov r12, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [r12]
    tst r3, #0x1000
    bne _L6c8
    orr r3, r3, #0x10
    str r3, [r12]
    tst r3, #0x800000
    bne _L184
    tst r3, #0x400000
    bne _L170
    tst r2, #0x80000000
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, r5, r6, r7, lr}
    bxeq lr
    movs r2, r2, lsl #1
    andeqs r2, r0, #1
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, r5, r6, r7, lr}
    bxeq lr
_L138:
    adds r0, r0, #1
    adc r1, r1, #0
    mov r2, r1, lsl #1
    cmn r2, #0x200000
    addlo sp, sp, #0x10
    ldmloia sp!, {r4, r5, r6, r7, lr}
    bxlo lr
    tst r3, #0x400
    bne _L63c
    orr r3, r3, #0x14
    str r3, [r12]
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L170:
    cmp lr, #0
    beq _L138
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L184:
    tst r3, #0x400000
    addne sp, sp, #0x10
    ldmneia sp!, {r4, r5, r6, r7, lr}
    bxne lr
    cmp lr, #0
    bne _L138
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L1a8:
    bics r12, r12, #0x800
    beq _L204
    orrs r6, r0, r1, lsl #1
    bne _L43c
    mov r4, r3, lsr #0x14
    mov r3, r3, lsl #0xb
    orr r3, r3, r2, lsr #21
    mov r2, r2, lsl #0xb
    movs r5, r4, lsl #0x15
    beq _L1e4
    cmn r5, #0x200000
    bne _L424
    orrs r5, r2, r3, lsl #1
    beq _L424
    b _L46c
_L1e4:
    orrs r5, r3, r2
    beq _L4bc
    b _L424
_L1f0:
    bics r4, r4, #0x800
    beq _L298
    orrs r6, r2, r3, lsl #1
    bne _L46c
    b _L424
_L204:
    orrs r6, r0, r1, lsl #1
    beq _L26c
    mov r12, #1
    cmp r1, #0
    bne _L228
    sub r12, r12, #0x20
    movs r1, r0
    mov r0, #0
    bmi _L244
_L228:
    clz r6, r1
    movs r1, r1, lsl r6
    rsb r6, r6, #0x20
    orr r1, r1, r0, lsr r6
    rsb r6, r6, #0x20
    mov r0, r0, lsl r6
    sub r12, r12, r6
_L244:
    mov r4, r3, lsr #0x14
    mov r3, r3, lsl #0xb
    orr r3, r3, r2, lsr #21
    mov r2, r2, lsl #0xb
    movs r5, r4, lsl #0x15
    cmnne r5, #0x200000
    beq _L1f0
    orr r3, r3, #0x80000000
    bic r4, r4, #0x800
    b _L054
_L26c:
    mov r4, r3, lsr #0x14
    mov r3, r3, lsl #0xb
    orr r3, r3, r2, lsr #21
    mov r2, r2, lsl #0xb
    movs r5, r4, lsl #0x15
    beq _L628
    cmn r5, #0x200000
    bne _L628
    orrs r6, r2, r3, lsl #1
    beq _L4bc
    b _L46c
_L298:
    orrs r5, r2, r3, lsl #1
    beq _L628
    mov r4, #1
    cmp r3, #0
    bne _L2bc
    sub r4, r4, #0x20
    movs r3, r2
    mov r2, #0
    bmi _L054
_L2bc:
    clz r6, r3
    movs r3, r3, lsl r6
    rsb r6, r6, #0x20
    orr r3, r3, r2, lsr r6
    rsb r6, r6, #0x20
    mov r2, r2, lsl r6
    sub r4, r4, r6
    b _L054
_L2dc:
    stmdb sp!, {r0, r1, r3, r12, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, r12, lr}
    ldr r2, [r2]
    tst r2, #0x800
    bne _L680
    cmn r12, #0x34
    beq _L41c
    bmi _L548
    mov r2, r1
    mov r3, r0
    add r4, r12, #0x34
    cmp r4, #0x20
    movge r2, r3
    movge r3, #0
    subge r4, r4, #0x20
    rsb r5, r4, #0x20
    mov r2, r2, lsl r4
    orr r2, r2, r3, lsr r5
    movs r3, r3, lsl r4
    orrne r2, r2, #1
    rsb r12, r12, #0xc
    cmp r12, #0x20
    movge r0, r1
    movge r1, #0
    subge r12, r12, #0x20
    rsb r4, r12, #0x20
    mov r0, r0, lsr r12
    orr r0, r0, r1, lsl r4
    orr r1, lr, r1, lsr r12
    cmp r2, #0
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, r5, r6, r7, lr}
    bxeq lr
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov r12, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [r12]
    tst r3, #0x1000
    bne _L6c8
    orr r3, r3, #0x18
    str r3, [r12]
    tst r3, #0x800000
    bne _L3f8
    tst r3, #0x400000
    bne _L3d4
    tst r2, #0x80000000
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, r5, r6, r7, lr}
    bxeq lr
    movs r2, r2, lsl #1
    andeqs r2, r0, #1
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, r5, r6, r7, lr}
    bxeq lr
    adds r0, r0, #1
    adc r1, r1, #0
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L3d4:
    cmp lr, #0
    addne sp, sp, #0x10
    ldmneia sp!, {r4, r5, r6, r7, lr}
    bxne lr
    adds r0, r0, #1
    adc r1, r1, #0
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L3f8:
    tst r3, #0x400000
    addne sp, sp, #0x10
    ldmneia sp!, {r4, r5, r6, r7, lr}
    bxne lr
    adds r0, r0, r1, lsr #31
    adc r1, r1, #0
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L41c:
    orr r0, r0, r1, lsl #1
    b _L4f4
_L424:
    ldr r1, =0x7ff00000
    orr r1, lr, r1
    mov r0, #0
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L43c:
    tst r1, #0x40000000
    beq _L4bc
    mov r4, r3, lsr #0x14
    mov r3, r3, lsl #0xb
    orr r3, r3, r2, lsr #21
    mov r2, r2, lsl #0xb
    movs r5, r4, lsl #0x15
    beq _L4e0
    cmn r5, #0x200000
    bne _L4e0
    orrs r5, r2, r3, lsl #1
    beq _L4e0
_L46c:
    mov r1, r3
    mov r3, r1
    mvn r0, #0
    bic r1, r0, #0x80000000
    tst r3, #0x40000000
    addne sp, sp, #0x10
    ldmneia sp!, {r4, r5, r6, r7, lr}
    bxne lr
    stmdb sp!, {r0, r1, r3, r12, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, r12, lr}
    ldr r3, [r2]
    tst r3, #0x100
    bne _L6ac
    orr r3, r3, #1
    str r3, [r2]
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L4bc:
    stmdb sp!, {r0, r1, r3, r12, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, r12, lr}
    ldr r3, [r2]
    tst r3, #0x100
    bne _L6ac
    orr r3, r3, #1
    str r3, [r2]
_L4e0:
    mvn r0, #0
    bic r1, r0, #0x80000000
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L4f4:
    movs r2, r0
    mov r1, lr
    mov r0, #0
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov r12, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [r12]
    tst r3, #0x1000
    bne _L6c8
    orr r3, r3, #0x18
    str r3, [r12]
    tst r3, #0x800000
    bne _L3f8
    tst r3, #0x400000
    bne _L3d4
    cmp r2, #0
    addne r0, r0, #1
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L548:
    mov r1, lr
    mov r0, #0
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov r12, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [r12]
    tst r3, #0x1000
    bne _L6c8
    orr r3, r3, #0x18
    str r3, [r12]
    tst r3, #0x800000
    bne _L3f8
    tst r3, #0x400000
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, r5, r6, r7, lr}
    bxeq lr
    cmp r1, #0
    addeq r0, r0, #1
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L5a0:
    stmdb sp!, {r0, r1, r3, r12, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, r12, lr}
    ldr r3, [r2]
    tst r3, #0x400
    bne _L654
    orr r3, r3, #0x14
    str r3, [r2]
    ldr r1, =0x7ff00000
    orr r1, lr, r1
    mov r0, #0
    tst r3, #0x800000
    bne _L5fc
    tst r3, #0x400000
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, r5, r6, r7, lr}
    bxeq lr
    subs r0, r0, lr, lsr #31
    sbc r1, r1, #0
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L5fc:
    subs r0, r0, #1
    sbc r1, r1, #0
    tst r3, #0x400000
    addne sp, sp, #0x10
    ldmneia sp!, {r4, r5, r6, r7, lr}
    bxne lr
    adds r0, r0, lr, lsr #31
    adc r1, r1, #0
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L628:
    mov r1, lr
    mov r0, #0
    add sp, sp, #0x10
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L63c:
    add sp, sp, #0x10
    sub r1, r1, #0x60000000
    ldr r2, =0x10000093
    bl func_0206dd7c
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L654:
    add sp, sp, #0x10
    mov r0, r0, lsr #0xb
    orr r0, r0, r1, lsl #21
    add r1, r1, r1
    orr r1, lr, r1, lsr #12
    sub r12, r12, #0x600
    orr r1, r1, r12, lsl #20
    ldr r2, =0x10000093
    bl func_0206dd7c
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L680:
    add sp, sp, #0x10
    mov r0, r0, lsr #0xb
    orr r0, r0, r1, lsl #21
    add r1, r1, r1
    orr r1, lr, r1, lsr #12
    add r12, r12, #0x600
    orr r1, r1, r12, lsl #20
    ldr r2, =0x20000093
    bl func_0206dd7c
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L6ac:
    ldmia sp!, {r0, r1, r2, r3}
    ldr ip, =0x04000093
    stmdb sp!, {r12}
    bl func_0206dcd4
    add sp, sp, #4
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
_L6c8:
    add sp, sp, #0x10
    ldr r2, =0x40000093
    bl func_0206dd7c
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
}
