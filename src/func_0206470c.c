// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
typedef struct BitmapFind {
    int max;
    int result;
    int current;
    char pad10[4];
    unsigned int *bits;
} BitmapFind;

asm int func_0206470c(BitmapFind *self) {
    stmdb sp!, {r4, r5, r6, r7, r8, lr}
    ldr r1, [r0, #0xc]
    ldr r4, [r0, #4]
    add r8, r1, #1
    cmp r8, r4
    movge r8, #0
    ldr lr, [r0, #0x14]
    mov r1, r8, asr #5
    add r5, lr, r1, lsl #2
    mov r7, r8
    and r6, r8, #0x1f
    mov r1, #0
    mov r2, #1
loop:
    mov r3, r2, lsl r6
    ldr ip, [r5]
    ands r3, ip, r3
    beq found
    add r8, r8, #1
    cmp r8, r4
    bge wrap
    add r6, r6, #1
    cmp r6, #0x1f
    movgt r6, r1
    addgt r5, r5, #4
    b check
wrap:
    mov r8, r1
    mov r6, r1
    mov r5, lr
check:
    cmp r8, r7
    bne loop
    b fail
found:
    str r8, [r0, #8]
    mov r0, r8
    ldmia sp!, {r4, r5, r6, r7, r8, lr}
    bx lr
fail:
    mvn r0, #0
    ldmia sp!, {r4, r5, r6, r7, r8, lr}
    bx lr
}