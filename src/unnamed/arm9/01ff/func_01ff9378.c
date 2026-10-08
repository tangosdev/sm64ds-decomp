// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// func_01ff9378 at 0x01ff9378 (ITCM) -- single-precision float add. The compiler emits a
// call to this for every `+` on a float; toolchain runtime, shipped as assembly, no C
// source for it anywhere in the game. No zero-size alias names this address, so it is
// filed under its address, matching the arm9 twenty's convention.
//
// Operands arrive as two 32-bit floats in r0 and r1 (not a register pair for one value --
// the exponent sits at bit 23, the 8-bit IEEE-754 single field, not the bit-52 double
// field _dadd/func_01ff8e10 use two rows up) and the sum comes back in r0. Structurally
// this is _dadd's single-precision twin: the entry XORs the two sign bits, same sign
// falls into this function's own magnitude-ADD core, differing signs tail-jump into
// `func_01ffa594` (the float-subtract primitive, three rows down) at ITS magnitude-
// subtract core, `_fsub_core` (0x01ffa5a8, that function's header +0x14).
// `func_01ffa594` tail-jumps back the same way into `_fadd_core` (0x01ff938c, this
// function's header +0x14) on its own differing-sign path -- each function is the
// other's continuation on that path, which no C construct can express (there is no way
// to jump into another function's interior and keep executing with its register state).
// Each function also pushes a small literal (1 here, 2 in func_01ffa594) that is later
// OR'd into the packed exception code func_0206ddcc/func_0206dd30 receive, tagging which
// of the two operations raised the trap -- another thing a plain C signature cannot do.
// config/arm9/itcm/symbols.txt carries `_fadd_core` and `_fsub_core` as the two label
// aliases this cross-linkage needs.
//
// Every exceptional path reads the FP status word through func_0207322c and leaves
// through func_0206ddcc or func_0206dd30 with a packed exception code -- the same kind of
// helper the double-precision primitives use, confirming this is one hand-written
// runtime block, not compiled C.

extern int func_0207322c(void);
extern void func_0206ddcc(void);
extern void func_0206dd30(void);
extern void _fsub_core(void);

asm float func_01ff9378(float a, float b)
{
    mov r2, #1
    stmdb sp!, {r0, r1, r2}
    eors r2, r0, r1
    eormi r1, r1, #0x80000000
    bmi _fsub_core
    subs ip, r0, r1
    sublo r0, r0, ip
    addlo r1, r1, ip
    mov r2, #0x80000000
    mov r3, r0, lsr #0x17
    orr r0, r2, r0, lsl #8
    ands ip, r3, #0xff
    cmpne ip, #0xff
    beq _L148
    mov ip, r1, lsr #0x17
    orr r1, r2, r1, lsl #8
    ands r2, ip, #0xff
    beq _L188
_L048:
    subs ip, r3, ip
    beq _L068
    cmp ip, #0x1c
    movge ip, #0x1f
    rsb r2, ip, #0x20
    movs r2, r1, lsl r2
    mov r1, r1, lsr ip
    orrne r1, r1, #1
_L068:
    adds r0, r0, r1
    blo _L088
    and r1, r0, #1
    orr r0, r1, r0, rrx
    add r3, r3, #1
    and r2, r3, #0xff
    cmp r2, #0xff
    beq _L2d8
_L088:
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
    bne _L448
    orr r3, r3, #0x10
    str r3, [ip]
    tst r3, #0x800000
    bne _L12c
    tst r3, #0x400000
    bne _L11c
    tst r1, #0x80
    addeq sp, sp, #0xc
    bxeq lr
    ands r1, r1, #0x7f
    andeqs r1, r0, #1
    addeq sp, sp, #0xc
    bxeq lr
_L0f0:
    add r0, r0, #1
    mov r1, r0, lsl #1
    cmn r1, #0x1000000
    addlo sp, sp, #0xc
    bxlo lr
    tst r3, #0x400
    bne _L3f0
    orr r3, r3, #0x14
    str r3, [ip]
    add sp, sp, #0xc
    bx lr
_L11c:
    cmp r0, #0
    bpl _L0f0
    add sp, sp, #0xc
    bx lr
_L12c:
    tst r3, #0x400000
    addne sp, sp, #0xc
    bxne lr
    cmp r0, #0
    bmi _L0f0
    add sp, sp, #0xc
    bx lr
_L148:
    cmp r3, #0x100
    movge r2, #0x80000000
    movlt r2, #0
    ands r3, r3, #0xff
    beq _L1ac
    movs r0, r0, lsl #1
    bne _L384
    mov ip, r1, lsr #0x17
    mov r1, r1, lsl #9
    ands ip, ip, #0xff
    beq _L374
    cmp ip, #0xff
    blt _L374
    cmp r1, #0
    beq _L374
    b _L3ac
_L188:
    cmp r3, #0x100
    movge r2, #0x80000000
    movlt r2, #0
    and r3, r3, #0xff
    ands ip, ip, #0xff
    beq _L208
_L1a0:
    movs r1, r1, lsl #1
    bne _L3ac
    b _L374
_L1ac:
    movs r0, r0, lsl #1
    beq _L1e4
    mov r3, #1
    mov r0, r0, lsr #1
    mov ip, r1, lsr #0x17
    mov r1, r1, lsl #8
    ands ip, ip, #0xff
    beq _L208
    cmp ip, #0xff
    beq _L1a0
    orr r1, r1, #0x80000000
    orr r3, r3, r2, lsr #23
    orr ip, ip, r2, lsr #23
    b _L048
_L1e4:
    mov r3, r1, lsr #0x17
    mov r0, r1, lsl #9
    ands r3, r3, #0xff
    beq _L2b0
    cmp r3, #0xff
    blt _L2b0
    cmp r0, #0
    beq _L374
    b _L33c
_L208:
    movs r1, r1, lsl #1
    beq _L2bc
    mov r1, r1, lsr #1
    mov ip, #1
    orr r3, r3, r2, lsr #23
    orr ip, ip, r2, lsr #23
    cmp r0, #0
    bmi _L048
    adds r0, r0, r1
    blo _L23c
    and r1, r0, #1
    orr r0, r1, r0, rrx
    add ip, ip, #1
_L23c:
    cmp r0, #0
    subge ip, ip, #1
    ands r1, r0, #0xff
    add r0, r0, r0
    mov r0, r0, lsr #9
    orr r0, r0, ip, lsl #23
    addeq sp, sp, #0xc
    bxeq lr
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov ip, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [ip]
    tst r3, #0x1000
    bne _L448
    orr r3, r3, #0x10
    str r3, [ip]
    tst r3, #0x800000
    bne _L12c
    tst r3, #0x400000
    bne _L11c
    tst r1, #0x80
    addeq sp, sp, #0xc
    bxeq lr
    ands r1, r1, #0x7f
    andeqs r1, r0, #1
    addne r0, r0, #1
    add sp, sp, #0xc
    bx lr
_L2b0:
    mov r0, r1
    add sp, sp, #0xc
    bx lr
_L2bc:
    cmp r0, #0
    subges r3, r3, #1
    add r0, r0, r0
    orr r0, r2, r0, lsr #9
    orr r0, r0, r3, lsl #23
    add sp, sp, #0xc
    bx lr
_L2d8:
    cmp r3, #0x100
    movge r2, #0x80000000
    movlt r2, #0
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov ip, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [ip]
    tst r3, #0x400
    bne _L40c
    orr r3, r3, #0x14
    str r3, [ip]
    mov r0, #0xff000000
    orr r0, r2, r0, lsr #1
    tst r3, #0x800000
    bne _L328
    tst r3, #0x400000
    subne r0, r0, r2, lsr #31
    add sp, sp, #0xc
    bx lr
_L328:
    sub r0, r0, #1
    tst r3, #0x400000
    addeq r0, r0, r2, lsr #31
    add sp, sp, #0xc
    bx lr
_L33c:
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
_L374:
    mov r0, #0xff000000
    orr r0, r2, r0, lsr #1
    add sp, sp, #0xc
    bx lr
_L384:
    tst r0, #0x80000000
    beq _L3c0
    mov ip, r1, lsr #0x17
    mov r1, r1, lsl #9
    ands ip, ip, #0xff
    beq _L3e4
    cmp ip, #0xff
    blt _L3e4
    cmp r1, #0
    beq _L3e4
_L3ac:
    tst r1, #0x80000000
    beq _L3c0
    mvn r0, #0x80000000
    add sp, sp, #0xc
    bx lr
_L3c0:
    stmdb sp!, {r0, r1, r3, ip, lr}
    bl func_0207322c
    mov r2, r0
    ldmia sp!, {r0, r1, r3, ip, lr}
    ldr r3, [r2]
    tst r3, #0x100
    bne _L42c
    orr r3, r3, #1
    str r3, [r2]
_L3e4:
    mvn r0, #0x80000000
    add sp, sp, #0xc
    bx lr
_L3f0:
    add sp, sp, #0xc
    sub r0, r0, #0x60000000
    stmdb sp!, {lr}
    mov r1, #0x10000001
    bl func_0206ddcc
    ldmia sp!, {lr}
    bx lr
_L40c:
    add sp, sp, #0xc
    orr r0, r2, r0, lsr #8
    add r0, r0, #0x1f000000
    stmdb sp!, {lr}
    mov r1, #0x10000001
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
_L448:
    add sp, sp, #0xc
    stmdb sp!, {lr}
    mov r1, #0x40000001
    bl func_0206ddcc
    ldmia sp!, {lr}
    bx lr
}
