// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// HAND-ASM: instruction-scheduling floor on n*0x32000 vs w-round interleave.
// Exhaustive C search (div=21 best): umull+sign-mla of n*0x32000 always finishes
// mla before asr of the scale result when that result is in lr; store-first shapes
// get the mid-mul asr schedule but recolor the scale into ip (div=30). Ternary
// fake-deps get mid-mul asr with lr but flip umull RdLo/RdHi (div=25). Same class
// as known SMULL schedule / RdLo-RdHi floors (notes/mwccarm-codegen.md 6l).
// Sibling func_ov095_021357d8 matched from C; this tail schedule is the residual.

extern int Vec3_Dist(void *a, void *b);
extern short Vec3_HorzAngle(void *a, void *b);
extern int func_ov095_0213579c(void *self, void *p);
extern int AngleDiff(int a, int b);
extern short data_02082214[];

asm void func_ov095_02135cdc(void)
{
    stmdb sp!, {r4, r5, r6, r7, lr}
    sub sp, sp, #4
    mov r6, r1
    mov r7, r0
    add r4, r6, #0x5c
    mov r1, r4
    add r0, r7, #0x5c
    bl Vec3_Dist
    mov r5, r0
    mov r1, r4
    add r0, r7, #0x5c
    bl Vec3_HorzAngle
    mov r4, r0
    mov r2, #1
    mov r0, r7
    mov r1, r6
    strb r2, [r7, #0x326]
    bl func_ov095_0213579c
    mov r6, r0
    add r1, r7, #0x8c
    add r0, r7, #0x300
    ldrsh ip, [r0, #0x24]
    mov r0, r4
    smull r4, r3, r5, r6
    ldrsh r5, [r1]
    mov r2, #0x800
    adds r4, r4, r2
    add r2, r5, ip
    strh r2, [r1]
    ldrsh r1, [r7, #0x8e]
    adc r2, r3, #0
    mov r4, r4, lsr #12
    orr r4, r4, r2, lsl #20
    bl AngleDiff
    mov r0, r0, lsl #16
    mov r0, r0, asr #16
    mov r0, r0, lsl #16
    mov r0, r0, lsr #16
    mov r0, r0, asr #4
    mov r0, r0, lsl #1
    add r1, r0, #1
    add r0, r7, #0x300
    mov r3, r1, lsl #1
    ldrsh r0, [r0, #0x24]
    ldr r1, [pc, #0xf8]
    mov r2, #0
    ldrsh r3, [r1, r3]
    mov r1, #0x800
    smull r5, r3, r4, r3
    adds r5, r5, r1
    adc r3, r3, #0
    muls r0, r0, r4
    mov ip, r5, lsr #12
    orr ip, ip, r3, lsl #20
    bpl Lpos
    mov r0, #0xa3
    umull r5, r4, ip, r0
    mla r4, ip, r2, r4
    mov r3, ip, asr #31
    adds r1, r5, r1
    mla r4, r3, r0, r4
    adc r0, r4, r2
    mov lr, r1, lsr #12
    orr lr, lr, r0, lsl #20
    b Ljoin
Lpos:
    mov r0, #0x51
    umull r5, r4, ip, r0
    mla r4, ip, r2, r4
    mov r3, ip, asr #31
    adds r1, r5, r1
    mla r4, r3, r0, r4
    adc r0, r4, r2
    mov lr, r1, lsr #12
    orr lr, lr, r0, lsl #20
Ljoin:
    mov r2, #0x32000
    mov r0, #0
    umull ip, r5, r6, r2
    mla r5, r6, r0, r5
    mov r4, r6, asr #31
    mov r1, lr, asr #11
    mov r3, #0x800
    add r1, lr, r1, lsr #20
    adds r3, ip, r3
    mla r5, r4, r2, r5
    add r0, r7, #0x324
    ldrsh r4, [r0]
    mov r1, r1, lsl #4
    adc r2, r5, #0
    mov r3, r3, lsr #12
    add r4, r4, r1, asr #16
    orr r3, r3, r2, lsl #20
    mov r1, r3, asr #11
    strh r4, [r0]
    add r0, r7, #0x300
    add r1, r3, r1, lsr #20
    mov r1, r1, lsl #4
    ldrsh r2, [r0, #0x24]
    mov r3, r1, asr #16
    cmp r2, r3
    strgth r3, [r0, #0x24]
    add r0, r7, #0x300
    ldrsh r1, [r0, #0x24]
    rsb r2, r3, #0
    cmp r1, r2
    strlth r2, [r0, #0x24]
    add sp, sp, #4
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
    dcd data_02082214
}
