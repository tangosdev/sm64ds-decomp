// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// func_01ff8e10 at 0x01ff8e10 (ITCM) -- double-precision subtract. The compiler emits a
// call to this for every `-` on a double; it is the toolchain's own runtime, shipped as
// assembly, and there is no C source for it in the game. No zero-size alias names this
// address in config/arm9/itcm/symbols.txt (unlike its neighbours _dadd/_dmul), so it is
// filed under its address, matching the arm9 twenty's convention (e.g. func_0205a588.c).
//
// Operands arrive in r0:r1 and r2:r3 and the difference comes back in r0:r1, already
// outside the C ABI for doubles. This is _dadd's mirror image: the entry XORs the two
// sign bits into `ip`; same sign here means an effective magnitude SUBTRACT, so it falls
// straight into this function's own core (right after the header, no branch taken).
// Differing signs mean an effective magnitude ADD, so the taken branch tail-jumps into
// `_dadd` (0x01ff8000, four rows up) at ITS magnitude-add core, `_dadd_core` (0x01ff8010,
// _dadd's header +0x10). `_dadd` symmetrically tail-jumps back into this function's own
// core, `_dsub_core` (0x01ff8e20, this function's header +0x10), on ITS differing-sign
// path. A live entry into the middle of a sibling function's body -- and each function
// is, in turn, the other's continuation -- is not something any C construct can express.
// config/arm9/itcm/symbols.txt carries `_dadd_core` and `_dsub_core` as the two label
// aliases this cross-linkage needs.
//
// Every exceptional path reads the FP status word through func_0207322c and, when the
// matching trap is enabled, leaves through func_0206dd7c or func_0206dcd4 with a packed
// exception code -- the same helpers _dadd and func_01ff8708 (_dmul) use, confirming this
// is one hand-written runtime block, not compiled C.

extern int func_0207322c(void);
extern void func_0206dd7c(void);
extern void func_0206dcd4(void);
extern void _dadd_core(void);

asm double func_01ff8e10(double a, double b)
{
    stmdb sp!, {r0, r1, r2, r3, r4, lr}
    eors ip, r1, r3
    eormi r3, r3, #0x80000000
    bmi _dadd_core
    subs ip, r0, r2
    sbcs lr, r1, r3
    bhs _L030
    eor lr, lr, #0x80000000
    adds r2, r2, ip
    adc r3, r3, lr
    subs r0, r0, ip
    sbc r1, r1, lr
_L030:
    mov lr, #0x80000000
    mov ip, r1, lsr #0x14
    orr r1, lr, r1, lsl #11
    orr r1, r1, r0, lsr #21
    mov r0, r0, lsl #0xb
    movs r4, ip, lsl #0x15
    cmnne r4, #0x200000
    beq _L2d4
    mov r4, r3, lsr #0x14
    orr r3, lr, r3, lsl #11
    orr r3, r3, r2, lsr #21
    mov r2, r2, lsl #0xb
    movs lr, r4, lsl #0x15
    beq _L31c
_L068:
    subs r4, ip, r4
    beq _L1a4
    cmp r4, #0x20
    ble _L0a4
    cmp r4, #0x38
    movge r4, #0x3f
    sub r4, r4, #0x20
    rsb lr, r4, #0x20
    orrs lr, r2, r3, lsl lr
    mov r2, r3, lsr r4
    orrne r2, r2, #1
    subs r0, r0, r2
    sbcs r1, r1, #0
    bmi _L0cc
    b _L258
_L0a4:
    rsb lr, r4, #0x20
    movs lr, r2, lsl lr
    rsb lr, r4, #0x20
    mov r2, r2, lsr r4
    orr r2, r2, r3, lsl lr
    mov r3, r3, lsr r4
    orrne r2, r2, #1
    subs r0, r0, r2
    sbcs r1, r1, r3
    bpl _L258
_L0cc:
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
    bne _L548
    orr r3, r3, #0x10
    str r3, [ip]
    tst r3, #0x800000
    bne _L180
    tst r3, #0x400000
    bne _L15c
    tst r2, #0x80000000
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, lr}
    bxeq lr
    movs r2, r2, lsl #1
    andeqs r2, r0, #1
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, lr}
    bxeq lr
    adds r0, r0, #1
    adc r1, r1, #0
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L15c:
    cmp r1, #0
    addmi sp, sp, #0x10
    ldmmiia sp!, {r4, lr}
    bxmi lr
    adds r0, r0, #1
    adc r1, r1, #0
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L180:
    tst r3, #0x400000
    addne sp, sp, #0x10
    ldmneia sp!, {r4, lr}
    bxne lr
    adds r0, r0, r1, lsr #31
    adc r1, r1, #0
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L1a4:
    subs r0, r0, r2
    sbc r1, r1, r3
    orrs lr, r1, r0
    beq _L4e0
    mov lr, ip, lsl #0x14
    and lr, lr, #0x80000000
    bic ip, ip, #0x800
    cmp r1, #0
    bmi _L230
    bne _L1dc
    sub ip, ip, #0x20
    movs r1, r0
    mov r0, #0
    bmi _L1f8
_L1dc:
    clz r4, r1
    movs r1, r1, lsl r4
    rsb r4, r4, #0x20
    orr r1, r1, r0, lsr r4
    rsb r4, r4, #0x20
    mov r0, r0, lsl r4
    sub ip, ip, r4
_L1f8:
    cmp ip, #0
    bgt _L238
    rsb ip, ip, #0xc
    cmp ip, #0x20
    movge r0, r1
    movge r1, #0
    subge ip, ip, #0x20
    rsb r4, ip, #0x20
    mov r0, r0, lsr ip
    orr r0, r0, r1, lsl r4
    orr r1, lr, r1, lsr ip
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L230:
    cmp r1, #0
    subges ip, ip, #1
_L238:
    mov r0, r0, lsr #0xb
    orr r0, r0, r1, lsl #21
    add r1, r1, r1
    orr r1, lr, r1, lsr #12
    orr r1, r1, ip, lsl #20
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L258:
    mov lr, ip, lsl #0x14
    and lr, lr, #0x80000000
    bic ip, ip, #0x800
    cmp r1, #0
    bne _L27c
    sub ip, ip, #0x20
    movs r1, r0
    mov r0, #0
    bmi _L298
_L27c:
    clz r4, r1
    movs r1, r1, lsl r4
    rsb r4, r4, #0x20
    orr r1, r1, r0, lsr r4
    rsb r4, r4, #0x20
    mov r0, r0, lsl r4
    sub ip, ip, r4
_L298:
    cmp ip, #0
    orrgt ip, ip, lr, lsr #20
    bgt _L0cc
    rsb ip, ip, #0xc
    cmp ip, #0x20
    movge r0, r1
    movge r1, #0
    subge ip, ip, #0x20
    rsb r4, ip, #0x20
    mov r0, r0, lsr ip
    orr r0, r0, r1, lsl r4
    orr r1, lr, r1, lsr ip
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L2d4:
    cmp ip, #0x800
    movge lr, #0x80000000
    movlt lr, #0
    bics ip, ip, #0x800
    beq _L340
    orrs r4, r0, r1, lsl #1
    bne _L428
    mov r4, r3, lsr #0x14
    mov r3, r3, lsl #0xb
    orr r3, r3, r2, lsr #21
    mov r2, r2, lsl #0xb
    movs r4, r4, lsl #0x15
    beq _L410
    cmn r4, #0x200000
    bne _L410
    orrs r4, r2, r3, lsl #1
    beq _L4a8
    b _L458
_L31c:
    cmp r4, #0x800
    movge lr, #0x80000000
    movlt lr, #0
    bic ip, ip, #0x800
    bics r4, r4, #0x800
    beq _L3b8
    orrs r4, r2, r3, lsl #1
    bne _L458
    b _L410
_L340:
    orrs r4, r0, r1, lsl #1
    beq _L380
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
    beq _L31c
    orr r3, r3, #0x80000000
    orr ip, ip, lr, lsr #20
    b _L068
_L380:
    mov ip, r3, lsr #0x14
    mov r1, r3, lsl #0xb
    orr r1, r1, r2, lsr #21
    mov r0, r2, lsl #0xb
    movs r4, ip, lsl #0x15
    beq _L3ac
    cmn r4, #0x200000
    bne _L3d4
    orrs r4, r0, r1, lsl #1
    bne _L45c
    b _L410
_L3ac:
    orrs r4, r0, r1, lsl #1
    beq _L4e0
    b _L3d4
_L3b8:
    orrs r4, r2, r3, lsl #1
    beq _L3e8
    mov r4, #1
    bic r3, r3, #0x80000000
    orr ip, ip, lr, lsr #20
    orr r4, r4, lr, lsr #20
    b _L068
_L3d4:
    mov r1, r3
    mov r0, r2
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L3e8:
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
_L410:
    ldr r1, =0x7ff00000
    orr r1, lr, r1
    mov r0, #0
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L428:
    tst r1, #0x40000000
    beq _L4a8
    mov r4, r3, lsr #0x14
    mov r3, r3, lsl #0xb
    orr r3, r3, r2, lsr #21
    mov r2, r2, lsl #0xb
    movs r4, r4, lsl #0x15
    beq _L4cc
    cmn r4, #0x200000
    bne _L4cc
    orrs r4, r2, r3, lsl #1
    beq _L4cc
_L458:
    mov r1, r3
_L45c:
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
    bne _L52c
    orr r3, r3, #1
    str r3, [r2]
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L4a8:
    stmdb sp!, {r0, r1, r3, ip, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, ip, lr}
    ldr r3, [r2]
    tst r3, #0x100
    bne _L52c
    orr r3, r3, #1
    str r3, [r2]
_L4cc:
    mvn r0, #0
    bic r1, r0, #0x80000000
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L4e0:
    mov r1, #0
    mov r0, #0
    stmdb sp!, {r0, r1, r2, ip, lr}
    bl func_0207322c
    mov r3, r0
    ldmia sp!, {r0, r1, r2, ip, lr}
    ldr r3, [r3]
    tst r3, #0x800000
    addeq sp, sp, #0x10
    ldmeqia sp!, {r4, lr}
    bxeq lr
    tst r3, #0x400000
    addne sp, sp, #0x10
    ldmneia sp!, {r4, lr}
    bxne lr
    mov r1, #0x80000000
    add sp, sp, #0x10
    ldmia sp!, {r4, lr}
    bx lr
_L52c:
    ldmia sp!, {r0, r1, r2, r3}
    ldr ip, =0x04000092
    stmdb sp!, {ip}
    bl func_0206dcd4
    add sp, sp, #4
    ldmia sp!, {r4, lr}
    bx lr
_L548:
    add sp, sp, #0x10
    ldr r2, =0x40000092
    bl func_0206dd7c
    ldmia sp!, {r4, lr}
    bx lr
}
