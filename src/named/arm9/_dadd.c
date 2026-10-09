// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// _dadd at 0x01ff8000 (ITCM), also known as func_01ff8000 -- double-precision add. The
// compiler emits a call to this for every `+` on a double; it is the toolchain's own
// runtime, shipped as assembly, and there is no C source for it in the game.
//
// Operands arrive in r0:r1 and r2:r3 and the sum comes back in r0:r1, already outside the
// C ABI for doubles. The entry XORs the two sign bits into `ip`: same sign falls straight
// into this function's own magnitude-ADD core (right after the header, no branch taken);
// differing signs need a magnitude SUBTRACT instead, so the taken branch tail-jumps into
// `func_01ff8e10` (the double-subtract primitive two rows down) at its OWN magnitude-
// subtract core, `_dsub_core` (0x01ff8e20, its header's +0x10). `func_01ff8e10` is built
// the same way in reverse: its own same-sign case IS a subtract (falls into its local
// core) and its differing-sign case tail-jumps back into THIS function's magnitude-add
// core, `_dadd_core` (0x01ff8010, this function's header's +0x10) -- a live entry into
// the middle of a sibling function's body, which no C construct can express (there is no
// way to jump into another function's interior and keep executing with its register
// state, let alone have two functions each be the other's continuation).
// config/arm9/itcm/symbols.txt carries `_dadd_core` and `_dsub_core` as the two label
// aliases this cross-linkage needs.
//
// Every exceptional path reads the FP status word through func_0207322c and, when the
// matching trap is enabled, leaves through func_0206dd7c or func_0206dcd4 with a packed
// exception code -- the same helpers func_01ff8708 (_dmul) uses, confirming this is one
// hand-written runtime block, not compiled C.

extern int func_0207322c(void);
extern void func_0206dd7c(void);
extern void func_0206dcd4(void);
extern void _dsub_core(void);

asm double _dadd(double a, double b)
{
    stmdb sp!, {r0, r1, r2, r3, r4, lr}
    eors ip, r1, r3
    eormi r3, r3, #0x80000000
    bmi _dsub_core
    subs ip, r0, r2
    sbcs lr, r1, r3
    bhs _L02c
    adds r2, r2, ip
    adc r3, r3, lr
    subs r0, r0, ip
    sbc r1, r1, lr
_L02c:
    mov lr, #0x80000000
    mov ip, r1, lsr #0x14
    orr r1, lr, r1, lsl #11
    orr r1, r1, r0, lsr #21
    mov r0, r0, lsl #0xb
    movs r4, ip, lsl #0x15
    cmnne r4, #0x200000
    beq _L1d0
    mov r4, r3, lsr #0x14
    orr r3, lr, r3, lsl #11
    orr r3, r3, r2, lsr #21
    mov r2, r2, lsl #0xb
    movs lr, r4, lsl #0x15
    beq _L218
_L064:
    subs r4, ip, r4
    beq _L0bc
    cmp r4, #0x20
    ble _L0a0
    cmp r4, #0x38
    movge r4, #0x3f
    sub r4, r4, #0x20
    rsb lr, r4, #0x20
    orrs lr, r2, r3, lsl lr
    mov r2, r3, lsr r4
    orrne r2, r2, #1
    adds r0, r0, r2
    adcs r1, r1, #0
    blo _L0e4
    b _L0c8
_L0a0:
    rsb lr, r4, #0x20
    movs lr, r2, lsl lr
    rsb lr, r4, #0x20
    mov r2, r2, lsr r4
    orr r2, r2, r3, lsl lr
    mov r3, r3, lsr r4
    orrne r2, r2, #1
_L0bc:
    adds r0, r0, r2
    adcs r1, r1, r3
    blo _L0e4
_L0c8:
    add ip, ip, #1
    and r4, r0, #1
    movs r1, r1, rrx
    orr r0, r4, r0, rrx
    mov lr, ip, lsl #0x15
    cmn lr, #0x200000
    beq _L47c
_L0e4:
    movs r2, r0, lsl #0x15
    mov r0, r0, lsr #0xb
    orr r0, r0, r1, lsl #21
    add r1, r1, r1
    mov r1, r1, lsr #0xc
    orr r1, r1, ip, lsl #20
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, lr}
    bxeq lr
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov ip, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [ip]
    tst r3, #0x1000
    bne _L574
    orr r3, r3, #0x10
    str r3, [ip]
    tst r3, #0x800000
    bne _L1ac
    tst r3, #0x400000
    bne _L198
    tst r2, #0x80000000
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, lr}
    bxeq lr
    movs r2, r2, lsl #1
    andeqs r2, r0, #1
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, lr}
    bxeq lr
_L160:
    adds r0, r0, #1
    adc r1, r1, #0
    mov r2, r1, lsl #1
    cmn r2, #0x200000
    addlo sp, sp, #0x10
    ldmloia sp!, {r4, lr}
    bxlo lr
    tst r3, #0x400
    bne _L510
    orr r3, r3, #0x14
    str r3, [ip]
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L198:
    cmp r1, #0
    bpl _L160
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L1ac:
    tst r3, #0x400000
    addne sp, sp, #0x10
    ldmneia sp!, {r4, lr}
    bxne lr
    cmp r1, #0
    bmi _L160
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L1d0:
    cmp ip, #0x800
    movge lr, #0x80000000
    movlt lr, #0
    bics ip, ip, #0x800
    beq _L23c
    orrs r4, r0, r1, lsl #1
    bne _L3c4
    mov r4, r3, lsr #0x14
    mov r3, r3, lsl #0xb
    orr r3, r3, r2, lsr #21
    mov r2, r2, lsl #0xb
    movs r4, r4, lsl #0x15
    beq _L3ac
    cmn r4, #0x200000
    bne _L3ac
    orrs r4, r2, r3, lsl #1
    beq _L3ac
    b _L3f4
_L218:
    cmp r4, #0x800
    movge lr, #0x80000000
    movlt lr, #0
    bic ip, ip, #0x800
    bics r4, r4, #0x800
    beq _L2a8
    orrs r4, r2, r3, lsl #1
    bne _L3f4
    b _L3ac
_L23c:
    orrs r4, r0, r1, lsl #1
    beq _L27c
    mov ip, #1
    bic r1, r1, #0x80000000
    mov r4, r3, lsr #0x14
    mov r3, r3, lsl #0xb
    orr r3, r3, r2, lsr #21
    mov r2, r2, lsl #0xb
    movs r4, r4, lsl #0x15
    cmnne r4, #0x200000
    mov r4, r4, lsr #0x15
    orr r4, r4, lr, lsr #20
    beq _L218
    orr r3, r3, #0x80000000
    orr ip, ip, lr, lsr #20
    b _L064
_L27c:
    mov ip, r3, lsr #0x14
    mov r1, r3, lsl #0xb
    orr r1, r1, r2, lsr #21
    mov r0, r2, lsl #0xb
    movs r4, ip, lsl #0x15
    beq _L370
    cmn r4, #0x200000
    bne _L370
    orrs r4, r0, r1, lsl #1
    beq _L3ac
    b _L3f8
_L2a8:
    orrs r4, r2, r3, lsl #1
    beq _L384
    mov r4, #1
    bic r3, r3, #0x80000000
    cmp r1, #0
    bpl _L2cc
    orr ip, ip, lr, lsr #20
    orr r4, r4, lr, lsr #20
    b _L064
_L2cc:
    adds r0, r0, r2
    adcs r1, r1, r3
    blo _L2ec
    add ip, ip, #1
    and r4, r0, #1
    movs r1, r1, rrx
    mov r0, r0, rrx
    orr r0, r0, r4
_L2ec:
    cmp r1, #0
    subges ip, ip, #1
    movs r2, r0, lsl #0x15
    mov r0, r0, lsr #0xb
    orr r0, r0, r1, lsl #21
    add r1, r1, r1
    orr r1, lr, r1, lsr #12
    orr r1, r1, ip, lsl #20
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, lr}
    bxeq lr
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov ip, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [ip]
    tst r3, #0x1000
    bne _L574
    orr r3, r3, #0x10
    str r3, [ip]
    tst r3, #0x800000
    bne _L1ac
    tst r3, #0x400000
    bne _L198
    tst r2, #0x80000000
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, lr}
    bxeq lr
    movs r2, r2, lsl #1
    andeqs r2, r0, #1
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, lr}
    bxeq lr
_L370:
    mov r1, r3
    mov r0, r2
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L384:
    cmp r1, #0
    subges ip, ip, #1
    mov r0, r0, lsr #0xb
    orr r0, r0, r1, lsl #21
    add r1, r1, r1
    orr r1, lr, r1, lsr #12
    orr r1, r1, ip, lsl #20
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L3ac:
    ldr r1, =0x7ff00000
    orr r1, lr, r1
    mov r0, #0
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L3c4:
    tst r1, #0x40000000
    beq _L444
    mov r4, r3, lsr #0x14
    mov r3, r3, lsl #0xb
    orr r3, r3, r2, lsr #21
    mov r2, r2, lsl #0xb
    movs r4, r4, lsl #0x15
    beq _L468
    cmn r4, #0x200000
    bne _L468
    orrs r4, r2, r3, lsl #1
    beq _L468
_L3f4:
    mov r1, r3
_L3f8:
    mov r3, r1
    mvn r0, #0
    bic r1, r0, #0x80000000
    tst r3, #0x40000000
    addne sp, sp, #0x10
    ldmneia sp!, {r4, lr}
    bxne lr
    stmdb sp!, {r0, r1, r3, ip, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, ip, lr}
    ldr r3, [r2]
    tst r3, #0x100
    bne _L558
    orr r3, r3, #1
    str r3, [r2]
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L444:
    stmdb sp!, {r0, r1, r3, ip, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, ip, lr}
    ldr r3, [r2]
    tst r3, #0x100
    bne _L558
    orr r3, r3, #1
    str r3, [r2]
_L468:
    mvn r0, #0
    bic r1, r0, #0x80000000
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L47c:
    cmp ip, #0x800
    movge lr, #0x80000000
    movlt lr, #0
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov ip, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [ip]
    tst r3, #0x400
    bne _L528
    orr r3, r3, #0x14
    str r3, [ip]
    ldr r1, =0x7ff00000
    orr r1, lr, r1
    mov r0, #0
    tst r3, #0x800000
    bne _L4e4
    tst r3, #0x400000
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, lr}
    bxeq lr
    subs r0, r0, lr, lsr #31
    sbc r1, r1, #0
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L4e4:
    subs r0, r0, #1
    sbc r1, r1, #0
    tst r3, #0x400000
    addne sp, sp, #0x10
    ldmneia sp!, {r4, lr}
    bxne lr
    adds r0, r0, lr, lsr #31
    adc r1, r1, #0
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L510:
    add sp, sp, #0x10
    sub r1, r1, #0x60000000
    ldr r2, =0x10000091
    bl func_0206dd7c
    ldmia sp!, {r4, lr}
    bx lr
_L528:
    add sp, sp, #0x10
    mov r0, r0, lsr #0xb
    orr r0, r0, r1, lsl #21
    add r1, r1, r1
    mov r1, r1, lsr #0xc
    ldr r2, =0x000001ff
    orr r1, r1, r2, lsl #20
    orr r1, r1, lr
    ldr r2, =0x10000091
    bl func_0206dd7c
    ldmia sp!, {r4, lr}
    bx lr
_L558:
    ldmia sp!, {r0, r1, r2, r3}
    ldr ip, =0x04000091
    stmdb sp!, {ip}
    bl func_0206dcd4
    add sp, sp, #4
    ldmia sp!, {r4, lr}
    bx lr
_L574:
    add sp, sp, #0x10
    ldr r2, =0x40000091
    bl func_0206dd7c
    ldmia sp!, {r4, lr}
    bx lr
}
