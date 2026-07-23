asm void func_0204af3c(void)
{
    stmdb sp!, {r4, r5, r6, r7, lr}
    sub sp, sp, #4
    ldr r7, [r0, #0xc]
    ldr r3, [pc, #0xc4]
    str r7, [sp]
    ldr r1, [r0, #4]
    mov r2, r7, lsl #0x1c
    mov lr, r2, lsr #0x1c
    mov r1, r1, lsr #3
    orr r12, r1, lr, lsl #26
    mov r4, r7, lsl #0x18
    mov r5, r7, lsl #0x14
    mov r2, r7, lsl #0xf
    mov r4, r4, lsr #0x1c
    orr r12, r12, #0x40000000
    mov r6, r7, lsl #0x12
    orr r12, r12, r4, lsl #20
    mov r5, r5, lsr #0x1c
    mov r1, r7, lsl #0x10
    orr r12, r12, r5, lsl #23
    mov r4, r6, lsr #0x1e
    mov r5, r1, lsr #0x1e
    orr r1, r12, r4, lsl #16
    mov r2, r2, lsr #0x1f
    orr r1, r1, r5, lsl #18
    orr r1, r1, r2, lsl #29
    str r1, [r3]
    cmp lr, #2
    moveq r1, #1
    movne r1, #0
    ldr r2, [r0, #8]
    rsb r1, r1, #4
    mov r2, r2, lsr r1
    ldr r1, [pc, #0x50]
    ldr r12, [pc, #0x50]
    str r2, [r1]
    mov r2, #3
    ldr r1, [pc, #0x48]
    str r2, [r12]
    mov r3, #0
    str r3, [r1]
    ldrh r1, [r0, #0x10]
    ldrh r2, [r0, #0x12]
    ldr r0, [pc, #0x34]
    mov r1, r1, lsl #0xc
    str r1, [r0]
    mov r1, r2, lsl #0xc
    str r1, [r0]
    str r3, [r0]
    mov r0, #1
    str r0, [r12]
    add sp, sp, #4
    ldmia sp!, {r4, r5, r6, r7, lr}
    bx lr
    dcd 0x040004a8
    dcd 0x040004ac
    dcd 0x04000440
    dcd 0x04000454
    dcd 0x0400046c
}
