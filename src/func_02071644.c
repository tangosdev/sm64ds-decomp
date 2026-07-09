// HAND-ASM: backward digit-carry increment; ROM uses ip/r12 for base and a
// backward branch epilogue that mwccarm does not reproduce from C (floor_skip
// regperm). Byte-faithful asm block per func_02059468 policy.
asm void func_02071644(void *obj, int len) {
    add ip, r0, #5
    add r1, ip, r1
    sub r3, r1, #1
    mov r1, #0
lbl_top:
    ldrb r2, [r3]
    cmp r2, #9
    addlo r0, r2, #1
    strlob r0, [r3]
    bxlo lr
    cmp r3, ip
    bne lbl_carry
    mov r1, #1
    strb r1, [r3]
    add r1, r0, #2
    ldrsh r0, [r1]
    add r0, r0, #1
    strh r0, [r1]
    bx lr
lbl_carry:
    strb r1, [r3], #-1
    b lbl_top
}