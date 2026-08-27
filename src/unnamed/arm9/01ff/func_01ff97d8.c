// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// func_01ff97d8 at 0x01ff97d8 (ITCM) -- the largest routine in the soft-float runtime
// block at 0x01ff8000, 0xb6c bytes of straight-line double-precision work with no frame,
// no callee-saved discipline and no compiled control flow. Assembly in the original,
// like every other function in that block.
//
// It runs the same house style as _dadd and _dmul around it: 64-bit operands kept in
// register pairs, mantissa alignment done with explicit `lsl`/`lsr` pairs and carries
// chained by hand, and one straight-line tail per special case (zero, denormal, infinity,
// NaN, overflow, underflow) instead of a merged epilogue.
//
// The eight `bl func_0207322c` sites are the FP status word reads that set the inexact,
// overflow and underflow flags and then test the matching enable bits; when a trap is
// enabled the routine leaves through func_0206dd30 or func_0206dc7c carrying a packed
// exception code. Those ten calls and two pool words are its only outside references.

extern int func_0207322c(void);
extern void func_0206dd30(void);
extern void func_0206dc7c(void);

asm double func_01ff97d8(double a, double b)
{
    mov r3, #0xff000000
    cmp r3, r0, lsl #1
    mov r2, #3
    blo _L374
    cmp r3, r1, lsl #1
    blo _L388
    cmp r0, #0
    bicmi r0, r0, #0x80000000
    rsbmi r0, r0, #0
    cmp r1, #0
    bicmi r1, r1, #0x80000000
    rsbmi r1, r1, #0
    cmp r0, r1
    moveq r0, #0
    movlt r0, #1
    movgt r0, #2
    bx lr
    mov r3, #0xff000000
    cmp r3, r0, lsl #1
    cmphs r3, r1, lsl #1
    movlo r2, #3
    blo _L2bc
    cmp r0, #0
    bicmi r0, r0, #0x80000000
    rsbmi r0, r0, #0
    cmp r1, #0
    bicmi r1, r1, #0x80000000
    rsbmi r1, r1, #0
    cmp r0, r1
    moveq r0, #0
    movlt r0, #1
    movgt r0, #2
    bx lr
    mov r3, #0xff000000
    cmp r3, r0, lsl #1
    mov r2, #1
    blo _L374
    cmp r3, r1, lsl #1
    blo _L388
    orr r3, r0, r1
    movs r3, r3, lsl #1
    moveq r0, #0
    bxeq lr
    cmp r0, r1
    movne r0, #1
    moveq r0, #0
    bx lr
    mov r3, #0xff000000
    cmp r3, r0, lsl #1
    mov r2, #0
    blo _L374
    cmp r3, r1, lsl #1
    blo _L388
    orr r3, r0, r1
    movs r3, r3, lsl #1
    moveq r0, #1
    bxeq lr
    cmp r0, r1
    moveq r0, #1
    movne r0, #0
    bx lr
    mov r3, #0xff000000
    cmp r3, r0, lsl #1
    blo _L398
    cmp r3, r1, lsl #1
    blo _L3b0
    orr r3, r0, r1
    movs r3, r3, lsl #1
    bxeq lr
    cmp r0, r1
    bx lr
    mov r3, #0xff000000
    cmp r3, r0, lsl #1
    cmphs r3, r1, lsl #1
    movlo r2, #0
    blo _L2bc
    cmp r0, #0
    bicmi r0, r0, #0x80000000
    rsbmi r0, r0, #0
    cmp r1, #0
    bicmi r1, r1, #0x80000000
    rsbmi r1, r1, #0
    cmp r0, r1
    movlt r0, #1
    movge r0, #0
    bx lr
    mov r3, #0xff000000
    cmp r3, r0, lsl #1
    cmphs r3, r1, lsl #1
    movlo r2, #0
    blo _L2bc
    cmp r0, #0
    bicmi r0, r0, #0x80000000
    rsbmi r0, r0, #0
    cmp r1, #0
    bicmi r1, r1, #0x80000000
    rsbmi r1, r1, #0
    cmp r0, r1
    movle r0, #1
    movgt r0, #0
    bx lr
    mov r3, #0xff000000
    cmp r3, r0, lsl #1
    cmphs r3, r1, lsl #1
    blo _L314
    orr r3, r0, r1
    movs r3, r3, lsl #1
    cmpeq r0, r0
    bxeq lr
    orrs r2, r0, r1
    bmi _L1c4
    cmp r0, r1
    bx lr
_L1c4:
    cmp r1, r0
    bx lr
    mov r3, #0xff000000
    cmp r3, r0, lsl #1
    cmphs r3, r1, lsl #1
    movlo r2, #0
    blo _L2bc
    cmp r0, #0
    bicmi r0, r0, #0x80000000
    rsbmi r0, r0, #0
    cmp r1, #0
    bicmi r1, r1, #0x80000000
    rsbmi r1, r1, #0
    cmp r0, r1
    movgt r0, #1
    movle r0, #0
    bx lr
    mov r3, #0xff000000
    cmp r3, r0, lsl #1
    cmphs r3, r1, lsl #1
    movlo r2, #0
    blo _L2bc
    cmp r0, #0
    bicmi r0, r0, #0x80000000
    rsbmi r0, r0, #0
    cmp r1, #0
    bicmi r1, r1, #0x80000000
    rsbmi r1, r1, #0
    cmp r0, r1
    movge r0, #1
    movlt r0, #0
    bx lr
    mov r3, #0xff000000
    cmp r3, r0, lsl #1
    cmphs r3, r1, lsl #1
    blo _L344
    orr r3, r0, r1
    movs r3, r3, lsl #1
    cmpeq r0, r0
    bxeq lr
    orrs r2, r0, r1
    bmi _L274
    cmp r0, r1
    bx lr
_L274:
    cmp r1, r0
    bx lr
    mov r3, #0xff000000
    cmp r3, r0, lsl #1
    mov r2, #1
    blo _L374
    cmp r3, r1, lsl #1
    blo _L388
    mov r0, #0
    bx lr
    mov r3, #0xff000000
    cmp r3, r0, lsl #1
    mov r2, #0
    blo _L374
    cmp r3, r1, lsl #1
    blo _L388
    mov r0, #1
    bx lr
_L2bc:
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov r12, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [r12]
    tst r3, #0x100
    bne _Lb38
    orr r3, r3, #1
    str r3, [r12]
    mov r0, r2
    bx lr
_L2e8:
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov r12, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [r12]
    tst r3, #0x100
    bne _Lb38
    orr r3, r3, #1
    str r3, [r12]
    movs r2, #1
    bx lr
_L314:
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov r12, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [r12]
    tst r3, #0x100
    bne _Lb38
    orr r3, r3, #1
    str r3, [r12]
    mov r2, #0xff000000
    adds r2, r2, #0x10000000
    bx lr
_L344:
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov r12, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr r3, [r12]
    tst r3, #0x100
    bne _Lb38
    orr r3, r3, #1
    str r3, [r12]
    mov r2, #0
    adds r2, r2, #0
    bx lr
_L374:
    tst r0, #0x400000
    beq _L2bc
    cmp r3, r1, lsl #1
    movhs r0, r2
    bxhs lr
_L388:
    tst r1, #0x400000
    beq _L2bc
    mov r0, r2
    bx lr
_L398:
    tst r0, #0x400000
    beq _L2e8
    cmp r3, r1, lsl #1
    blo _L3b0
    movs r2, #1
    bx lr
_L3b0:
    tst r1, #0x400000
    beq _L2e8
    movs r2, #1
    bx lr
    mov r12, #0x200000
    cmn r12, r1, lsl #1
    bhs _L41c
    cmn r12, r3, lsl #1
    bhs _L438
_L3d4:
    orrs r12, r3, r1
    bmi _L3f4
    cmp r1, r3
    cmpeq r0, r2
    moveq r0, #0
    movlo r0, #1
    movhi r0, #2
    bx lr
_L3f4:
    orr r12, r0, r12, lsl #1
    orrs r12, r12, r2
    moveq r0, #0
    bxeq lr
    cmp r3, r1
    cmpeq r2, r0
    moveq r0, #0
    movlo r0, #1
    movhi r0, #2
    bx lr
_L41c:
    movne r12, #3
    bne _Lac8
    cmp r0, #0
    movhi r12, #3
    bhi _Lac8
    cmn r12, r3, lsl #1
    blo _L3d4
_L438:
    movne r12, #3
    bne _Laf0
    cmp r2, #0
    movhi r12, #3
    bhi _Laf0
    b _L3d4
    mov r12, #0x200000
    cmn r12, r1, lsl #1
    bhs _L4ac
    cmn r12, r3, lsl #1
    bhs _L4c8
_L464:
    orrs r12, r3, r1
    bmi _L484
    cmp r1, r3
    cmpeq r0, r2
    moveq r0, #0
    movlo r0, #1
    movhi r0, #2
    bx lr
_L484:
    orr r12, r0, r12, lsl #1
    orrs r12, r12, r2
    moveq r0, #0
    bxeq lr
    cmp r3, r1
    cmpeq r2, r0
    moveq r0, #0
    movlo r0, #1
    movhi r0, #2
    bx lr
_L4ac:
    movne r12, #3
    bne _L9e4
    cmp r0, #0
    movhi r12, #3
    bhi _L9e4
    cmn r12, r3, lsl #1
    blo _L464
_L4c8:
    movne r12, #3
    bne _L9e4
    cmp r2, #0
    movhi r12, #3
    bhi _L9e4
    b _L464
    mov r12, #0x200000
    cmn r12, r1, lsl #1
    bhs _L534
    cmn r12, r3, lsl #1
    bhs _L550
_L4f4:
    orrs r12, r3, r1
    bmi _L510
    cmp r1, r3
    cmpeq r0, r2
    movne r0, #1
    moveq r0, #0
    bx lr
_L510:
    orr r12, r0, r12, lsl #1
    orrs r12, r12, r2
    moveq r0, #0
    bxeq lr
    cmp r3, r1
    cmpeq r2, r0
    movne r0, #1
    moveq r0, #0
    bx lr
_L534:
    movne r12, #1
    bne _Lac8
    cmp r0, #0
    movhi r12, #1
    bhi _Lac8
    cmn r12, r3, lsl #1
    blo _L4f4
_L550:
    movne r12, #1
    bne _Laf0
    cmp r2, #0
    movhi r12, #1
    bhi _Laf0
    b _L4f4
    mov r12, #0x200000
    cmn r12, r1, lsl #1
    bhs _L5bc
    cmn r12, r3, lsl #1
    bhs _L5d8
_L57c:
    orrs r12, r3, r1
    bmi _L598
    cmp r1, r3
    cmpeq r0, r2
    moveq r0, #1
    movne r0, #0
    bx lr
_L598:
    orr r12, r0, r12, lsl #1
    orrs r12, r12, r2
    moveq r0, #1
    bxeq lr
    cmp r3, r1
    cmpeq r2, r0
    moveq r0, #1
    movne r0, #0
    bx lr
_L5bc:
    movne r12, #0
    bne _Lac8
    cmp r0, #0
    movhi r12, #0
    bhi _Lac8
    cmn r12, r3, lsl #1
    blo _L57c
_L5d8:
    movne r12, #0
    bne _Laf0
    cmp r2, #0
    movhi r12, #0
    bhi _Laf0
    b _L57c
    mov r12, #0x200000
    cmn r12, r1, lsl #1
    bhs _L630
    cmn r12, r3, lsl #1
    bhs _L644
_L604:
    orrs r12, r3, r1
    bmi _L618
    cmp r1, r3
    cmpeq r0, r2
    bx lr
_L618:
    orr r12, r0, r12, lsl #1
    orrs r12, r12, r2
    bxeq lr
    cmp r3, r1
    cmpeq r2, r0
    bx lr
_L630:
    bne _Lb00
    cmp r0, #0
    bhi _Lb00
    cmn r12, r3, lsl #1
    blo _L604
_L644:
    bne _Lb28
    cmp r2, #0
    bhi _Lb28
    b _L604
    mov r12, #0x200000
    cmn r12, r1, lsl #1
    bhs _L6a8
    cmn r12, r3, lsl #1
    bhs _L6c4
_L668:
    orrs r12, r3, r1
    bmi _L684
    cmp r1, r3
    cmpeq r0, r2
    movlo r0, #1
    movhs r0, #0
    bx lr
_L684:
    orr r12, r0, r12, lsl #1
    orrs r12, r12, r2
    moveq r0, #0
    bxeq lr
    cmp r3, r1
    cmpeq r2, r0
    movlo r0, #1
    movhs r0, #0
    bx lr
_L6a8:
    movne r12, #0
    bne _L9e4
    cmp r0, #0
    movhi r12, #0
    bhi _L9e4
    cmn r12, r3, lsl #1
    blo _L668
_L6c4:
    movne r12, #0
    bne _L9e4
    cmp r2, #0
    movhi r12, #0
    bhi _L9e4
    b _L668
    mov r12, #0x200000
    cmn r12, r1, lsl #1
    bhs _L730
    cmn r12, r3, lsl #1
    bhs _L74c
_L6f0:
    orrs r12, r3, r1
    bmi _L70c
    cmp r1, r3
    cmpeq r0, r2
    movls r0, #1
    movhi r0, #0
    bx lr
_L70c:
    orr r12, r0, r12, lsl #1
    orrs r12, r12, r2
    moveq r0, #1
    bxeq lr
    cmp r3, r1
    cmpeq r2, r0
    movls r0, #1
    movhi r0, #0
    bx lr
_L730:
    movne r12, #0
    bne _L9e4
    cmp r0, #0
    movhi r12, #0
    bhi _L9e4
    cmn r12, r3, lsl #1
    blo _L6f0
_L74c:
    movne r12, #0
    bne _L9e4
    cmp r2, #0
    movhi r12, #0
    bhi _L9e4
    b _L6f0
    mov r12, #0x200000
    cmn r12, r1, lsl #1
    bhs _L7a8
    cmn r12, r3, lsl #1
    bhs _L7bc
_L778:
    orrs r12, r3, r1
    bmi _L78c
    cmp r1, r3
    cmpeq r0, r2
    bx lr
_L78c:
    orr r12, r0, r12, lsl #1
    orrs r12, r12, r2
    cmpeq r1, r1
    bxeq lr
    cmp r3, r1
    cmpeq r2, r0
    bx lr
_L7a8:
    bne _La50
    cmp r0, #0
    bhi _La50
    cmn r12, r3, lsl #1
    blo _L778
_L7bc:
    bne _La50
    cmp r2, #0
    bhi _La50
    b _L778
    mov r12, #0x200000
    cmn r12, r1, lsl #1
    bhs _L820
    cmn r12, r3, lsl #1
    bhs _L83c
_L7e0:
    orrs r12, r3, r1
    bmi _L7fc
    cmp r1, r3
    cmpeq r0, r2
    movhi r0, #1
    movls r0, #0
    bx lr
_L7fc:
    orr r12, r0, r12, lsl #1
    orrs r12, r12, r2
    moveq r0, #0
    bxeq lr
    cmp r3, r1
    cmpeq r2, r0
    movhi r0, #1
    movls r0, #0
    bx lr
_L820:
    movne r12, #0
    bne _L9e4
    cmp r0, #0
    movhi r12, #0
    bhi _L9e4
    cmn r12, r3, lsl #1
    blo _L7e0
_L83c:
    movne r12, #0
    bne _L9e4
    cmp r2, #0
    movhi r12, #0
    bhi _L9e4
    b _L7e0
    mov r12, #0x200000
    cmn r12, r1, lsl #1
    bhs _L8a8
    cmn r12, r3, lsl #1
    bhs _L8c4
_L868:
    orrs r12, r3, r1
    bmi _L884
    cmp r1, r3
    cmpeq r0, r2
    movhs r0, #1
    movlo r0, #0
    bx lr
_L884:
    orr r12, r0, r12, lsl #1
    orrs r12, r12, r2
    moveq r0, #1
    bxeq lr
    cmp r3, r1
    cmpeq r2, r0
    movhs r0, #1
    movlo r0, #0
    bx lr
_L8a8:
    movne r12, #0
    bne _L9e4
    cmp r0, #0
    movhi r12, #0
    bhi _L9e4
    cmn r12, r3, lsl #1
    blo _L868
_L8c4:
    movne r12, #0
    bne _L9e4
    cmp r2, #0
    movhi r12, #0
    bhi _L9e4
    b _L868
    mov r12, #0x200000
    cmn r12, r1, lsl #1
    bhs _L920
    cmn r12, r3, lsl #1
    bhs _L934
_L8f0:
    orrs r12, r3, r1
    bmi _L904
    cmp r1, r3
    cmpeq r0, r2
    bx lr
_L904:
    orr r12, r0, r12, lsl #1
    orrs r12, r12, r2
    cmpeq r1, r1
    bxeq lr
    cmp r3, r1
    cmpeq r2, r0
    bx lr
_L920:
    bne _La8c
    cmp r0, #0
    bhi _La8c
    cmn r12, r3, lsl #1
    blo _L8f0
_L934:
    bne _La8c
    cmp r2, #0
    bhi _La8c
    b _L8f0
    mov r12, #0x200000
    cmn r12, r1, lsl #1
    bhs _L960
    cmn r12, r3, lsl #1
    bhs _L97c
_L958:
    mov r0, #0
    bx lr
_L960:
    movne r12, #1
    bne _Lac8
    cmp r0, #0
    movhi r12, #1
    bhi _Lac8
    cmn r12, r3, lsl #1
    blo _L958
_L97c:
    movne r12, #1
    bne _Laf0
    cmp r2, #0
    movhi r12, #1
    bhi _Laf0
    b _L958
    mov r12, #0x200000
    cmn r12, r1, lsl #1
    bhs _L9b0
    cmn r12, r3, lsl #1
    bhs _L9cc
_L9a8:
    mov r0, #1
    bx lr
_L9b0:
    movne r12, #0
    bne _Lac8
    cmp r0, #0
    movhi r12, #0
    bhi _Lac8
    cmn r12, r3, lsl #1
    blo _L9a8
_L9cc:
    movne r12, #0
    bne _Laf0
    cmp r2, #0
    movhi r12, #0
    bhi _Laf0
    b _L9a8
_L9e4:
    stmdb sp!, {r12, lr}
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov r12, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr lr, [r12]
    tst lr, #0x100
    bne _Lb4c
    orr lr, lr, #1
    str lr, [r12]
    ldmia sp!, {r0}
    ldmia sp!, {lr}
    bx lr
_La18:
    stmdb sp!, {r12, lr}
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov r12, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr lr, [r12]
    tst lr, #0x100
    bne _Lb4c
    orr lr, lr, #1
    str lr, [r12]
    add sp, sp, #4
    movs r12, #1
    ldmia sp!, {lr}
    bx lr
_La50:
    stmdb sp!, {r12, lr}
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov r12, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr lr, [r12]
    tst lr, #0x100
    bne _Lb4c
    orr lr, lr, #1
    str lr, [r12]
    add sp, sp, #4
    mov r12, #0xff000000
    adds r12, r12, #0x10000000
    ldmia sp!, {lr}
    bx lr
_La8c:
    stmdb sp!, {r12, lr}
    stmdb sp!, {r0, r1, r2, r3, lr}
    bl func_0207322c
    mov r12, r0
    ldmia sp!, {r0, r1, r2, r3, lr}
    ldr lr, [r12]
    tst lr, #0x100
    bne _Lb4c
    orr lr, lr, #1
    str lr, [r12]
    add sp, sp, #4
    mov r12, #0
    adds r12, r12, #0
    ldmia sp!, {lr}
    bx lr
_Lac8:
    tst r1, #0x80000
    beq _L9e4
    stmdb sp!, {r12}
    mov r12, #0x200000
    cmn r12, r3, lsl #1
    ldmia sp!, {r12}
    blo _Laf8
    bne _Laf0
    cmp r2, #0
    beq _Laf8
_Laf0:
    tst r3, #0x80000
    beq _L9e4
_Laf8:
    mov r0, r12
    bx lr
_Lb00:
    tst r1, #0x80000
    beq _La18
    stmdb sp!, {r12}
    mov r12, #0x200000
    cmn r12, r3, lsl #1
    ldmia sp!, {r12}
    blo _Lb30
    bne _Lb28
    cmp r2, #0
    beq _Lb30
_Lb28:
    tst r3, #0x80000
    beq _La18
_Lb30:
    movs r12, #1
    bx lr
_Lb38:
    stmdb sp!, {lr}
    ldr r2, =0x04000049
    bl func_0206dd30
    ldmia sp!, {lr}
    bx lr
_Lb4c:
    ldr ip, =0x040000c9
    stmdb sp!, {r12}
    bl func_0206dc7c
    add sp, sp, #8
    ldmia sp!, {lr}
    bx lr
}
