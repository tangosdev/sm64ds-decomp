// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// func_01ffa594 at 0x01ffa594 (ITCM) -- single-precision float subtract. The compiler
// emits a call to this for every `-` on a float; toolchain runtime, shipped as assembly,
// no C source for it anywhere in the game. No zero-size alias names this address, so it
// is filed under its address, matching the arm9 twenty's convention.
//
// This is func_01ff9378's (float-add) mirror image, the same relationship _dadd and
// func_01ff8e10 (double add/subtract) have four rows up: operands arrive as two 32-bit
// floats in r0 and r1, the difference comes back in r0. The entry XORs the two sign
// bits; same sign here means an effective magnitude SUBTRACT, so it falls into this
// function's own core. Differing signs mean an effective magnitude ADD, so the taken
// branch tail-jumps into `func_01ff9378` at ITS magnitude-add core, `_fadd_core`
// (0x01ff938c, that function's header +0x14); `func_01ff9378` tail-jumps back into this
// function's own core, `_fsub_core` (0x01ffa5a8, this function's header +0x14), on its
// own differing-sign path -- each function is, in turn, the other's continuation, which
// no C construct can express. It also pushes the literal 2 (func_01ff9378 pushes 1) that
// later gets OR'd into the packed exception code, tagging which operation raised the
// trap. config/arm9/itcm/symbols.txt carries `_fadd_core` and `_fsub_core` as the two
// label aliases this cross-linkage needs.
//
// Every exceptional path reads the FP status word through func_0207322c and leaves
// through func_0206ddcc or func_0206dd30 with a packed exception code, the same helpers
// func_01ff9378 uses, confirming this is one hand-written runtime block, not compiled C.

extern int func_0207322c(void);
extern void func_0206ddcc(void);
extern void func_0206dd30(void);
extern void _fadd_core(void);

asm float func_01ffa594(float a, float b)
{
    mov r2, #2
    stmdb sp!, {r0, r1, r2}
    eors r2, r0, r1
    eormi r1, r1, #0x80000000
    bmi _fadd_core
    subs ip, r0, r1
    eorlo ip, ip, #0x80000000
    sublo r0, r0, ip
    addlo r1, r1, ip
    mov r2, #0x80000000
    mov r3, r0, lsr #0x17
    orr r0, r2, r0, lsl #8
    ands ip, r3, #0xff
    cmpne ip, #0xff
    beq _L1f4
    mov ip, r1, lsr #0x17
    orr r1, r2, r1, lsl #8
    ands r2, ip, #0xff
    beq _L234
_L04c:
    subs ip, r3, ip
    beq _L10c
    cmp ip, #0x1c
    movge ip, #0x1f
    rsb r2, ip, #0x20
    movs r2, r1, lsl r2
    mov r1, r1, lsr ip
    orrne r1, r1, #1
    subs r0, r0, r1
    bpl _L158
    ands r1, r0, #0xff
    add r0, r0, r0
    mov r0, r0, lsr #9
    orr r0, r0, r3, lsl #23
    addeq sp, sp, #0xc
    bxeq lr
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov ip, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [ip]
    tst r3, #0x1000
    bne _L414
    orr r3, r3, #0x10
    str r3, [ip]
    tst r3, #0x800000
    bne _L0f0
    tst r3, #0x400000
    bne _L0e0
    tst r1, #0x80
    addeq sp, sp, #0xc
    bxeq lr
    ands r1, r1, #0x7f
    andeqs r1, r0, #1
    addne r0, r0, #1
    add sp, sp, #0xc
    bx lr
_L0e0:
    cmp r0, #0
    addpl r0, r0, #1
    add sp, sp, #0xc
    bx lr
_L0f0:
    tst r3, #0x400000
    addne sp, sp, #0xc
    bxne lr
    cmp r0, #0
    addmi r0, r0, #1
    add sp, sp, #0xc
    bx lr
_L10c:
    subs r0, r0, r1
    beq _L308
    mov r2, r3, lsl #0x17
    and r2, r2, #0x80000000
    bic r3, r3, #0x100
    clz ip, r0
    movs r0, r0, lsl ip
    sub r3, r3, ip
    cmp r3, #0
    bgt _L144
    rsb r3, r3, #9
    orr r0, r2, r0, lsr r3
    add sp, sp, #0xc
    bx lr
_L144:
    add r0, r0, r0
    orr r0, r2, r0, lsr #9
    orr r0, r0, r3, lsl #23
    add sp, sp, #0xc
    bx lr
_L158:
    mov r2, r3, lsl #0x17
    and r2, r2, #0x80000000
    bic r3, r3, #0x100
    clz ip, r0
    movs r0, r0, lsl ip
    sub r3, r3, ip
    cmp r3, #0
    bgt _L188
    rsb r3, r3, #9
    orr r0, r2, r0, lsr r3
    add sp, sp, #0xc
    bx lr
_L188:
    ands r1, r0, #0xff
    add r0, r0, r0
    orr r0, r2, r0, lsr #9
    orr r0, r0, r3, lsl #23
    addeq sp, sp, #0xc
    bxeq lr
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov ip, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [ip]
    tst r3, #0x1000
    bne _L414
    orr r3, r3, #0x10
    str r3, [ip]
    tst r3, #0x800000
    bne _L0f0
    tst r3, #0x400000
    bne _L0e0
    tst r1, #0x80
    addeq sp, sp, #0xc
    bxeq lr
    ands r1, r1, #0x7f
    andeqs r1, r0, #1
    addne r0, r0, #1
    add sp, sp, #0xc
    bx lr
_L1f4:
    cmp r3, #0x100
    movge r2, #0x80000000
    movlt r2, #0
    ands r3, r3, #0xff
    beq _L25c
    movs r0, r0, lsl #1
    bne _L3a8
    mov ip, r1, lsr #0x17
    mov r1, r1, lsl #9
    ands ip, ip, #0xff
    beq _L398
    cmp ip, #0xff
    blt _L398
    cmp r1, #0
    beq _L3e4
    b _L3d0
_L234:
    cmp ip, #0x100
    movge r2, #0x80000000
    movlt r2, #0
    and r3, r3, #0xff
    ands ip, ip, #0xff
    beq _L2c4
_L24c:
    eor r2, r2, #0x80000000
    movs r1, r1, lsl #1
    bne _L3d0
    b _L398
_L25c:
    movs r0, r0, lsl #1
    beq _L294
    mov r0, r0, lsr #1
    mov r3, #1
    mov ip, r1, lsr #0x17
    mov r1, r1, lsl #8
    ands ip, ip, #0xff
    beq _L2c4
    cmp ip, #0xff
    beq _L24c
    orr r1, r1, #0x80000000
    orr r3, r3, r2, lsr #23
    orr ip, ip, r2, lsr #23
    b _L04c
_L294:
    mov r3, r1, lsr #0x17
    mov r0, r1, lsl #9
    ands r2, r3, #0xff
    beq _L2b8
    cmp r2, #0xff
    blt _L2e0
    cmp r0, #0
    bne _L360
    b _L398
_L2b8:
    cmp r0, #0
    beq _L308
    b _L2e0
_L2c4:
    movs r1, r1, lsl #1
    beq _L2ec
    mov r1, r1, lsr #1
    mov ip, #1
    orr ip, ip, r2, lsr #23
    orr r3, r3, r2, lsr #23
    b _L04c
_L2e0:
    mov r0, r1
    add sp, sp, #0xc
    bx lr
_L2ec:
    cmp r0, #0
    subges r3, r3, #1
    add r0, r0, r0
    orr r0, r2, r0, lsr #9
    orr r0, r0, r3, lsl #23
    add sp, sp, #0xc
    bx lr
_L308:
    mov r0, #0
    stmdb sp!, {r0, r1, r2, ip, lr}
    bl func_0207322c
    mov r3, r0
    ldmia sp!, {r0, r1, r2, ip, lr}
    ldr r3, [r3]
    tst r3, #0x800000
    addeq sp, sp, #0xc
    bxeq lr
    tst r3, #0x400000
    addne sp, sp, #0xc
    bxne lr
    mov r0, #0x80000000
    add sp, sp, #0xc
    bx lr
    cmp r0, #0
    subges r3, r3, #1
    add r0, r0, r0
    mov r0, r0, lsr #9
    orr r0, r0, r3, lsl #23
    add sp, sp, #0xc
    bx lr
_L360:
    mov r3, r0
    mvn r0, #0x80000000
    tst r3, #0x80000000
    addne sp, sp, #0xc
    bxne lr
    stmdb sp!, {r0, r1, r3, ip, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, ip, lr}
    ldr r3, [r2]
    orr r3, r3, #1
    str r3, [r2]
    add sp, sp, #0xc
    bx lr
_L398:
    mov r0, #0xff000000
    orr r0, r2, r0, lsr #1
    add sp, sp, #0xc
    bx lr
_L3a8:
    tst r0, #0x80000000
    beq _L3e4
    mov ip, r1, lsr #0x17
    mov r1, r1, lsl #9
    ands ip, ip, #0xff
    beq _L408
    cmp ip, #0xff
    blt _L408
    cmp r1, #0
    beq _L408
_L3d0:
    tst r1, #0x80000000
    beq _L3e4
    mvn r0, #0x80000000
    add sp, sp, #0xc
    bx lr
_L3e4:
    stmdb sp!, {r0, r1, r3, ip, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, ip, lr}
    ldr r3, [r2]
    tst r3, #0x100
    bne _L42c
    orr r3, r3, #1
    str r3, [r2]
_L408:
    mvn r0, #0x80000000
    add sp, sp, #0xc
    bx lr
_L414:
    add sp, sp, #0xc
    stmdb sp!, {lr}
    mov r1, #0x40000002
    bl func_0206ddcc
    ldmia sp!, {lr}
    bx lr
_L42c:
    ldmia sp!, {r0, r1, r2}
    stmdb sp!, {lr}
    mov ip, #0x4000000
    orr r2, r2, ip
    bl func_0206dd30
    ldmia sp!, {lr}
    bx lr
}
