asm void func_ov080_02125460(void)
{
    stmdb sp!, {r4, lr}
    ldr r1, [r0, #8]
    ldr r2, [r0, #0x1a8]
    mov r1, r1, lsr #0xd
    and r1, r1, #3
    and r1, r1, #0xff
    cmp r1, #2
    ldr r3, [r2, #8]
    bhs L_high
    add r1, r0, #0x100
    ldrh r1, [r1, #0xb6]
    cmp r1, #0
    bne L_nonzero
L_high:
    ldr r1, [pc, #0xa4]
    orr r3, r3, r1
L_nonzero:
    ldr r1, [pc, #0xa0]
    ldr r2, [pc, #0xa0]
    str r3, [r1]
    ldr r3, [r0, #0x1a8]
    ldr r1, [pc, #0x98]
    ldr r3, [r3, #0xc]
    str r3, [r2]
    ldr r2, [r0, #0x1a8]
    ldr r4, [r2, #0x10]
    ldr r2, [r2]
    mov r3, r4, lsr #0x1a
    mov r12, r4, lsr #0x14
    mov lr, r4, lsr #0x17
    mov r4, r4, lsr #0x1d
    and r3, r3, #7
    mov r2, r2, lsr #3
    orr r2, r2, r3, lsl #26
    and r12, r12, #7
    and r3, lr, #7
    orr r2, r2, r12, lsl #20
    and r4, r4, #1
    orr r2, r2, r3, lsl #23
    orr r2, r2, r4, lsl #29
    str r2, [r1]
    ldr r1, [r0, #0x1a8]
    ldr r2, [pc, #0x48]
    ldr r0, [r1, #0x10]
    ldr r1, [r1, #4]
    mov r0, r0, lsr #0x1a
    and r0, r0, #7
    cmp r0, #2
    moveq r0, #1
    movne r0, #0
    rsb r0, r0, #4
    mov r3, r1, lsr r0
    ldr r1, [pc, #0x24]
    ldr r0, [pc, #0x24]
    str r3, [r2]
    str r1, [r0]
    ldmia sp!, {r4, lr}
    bx lr
    dcd 0x7fff0000
    dcd 0x040004c0
    dcd 0x040004c4
    dcd 0x040004a8
    dcd 0x040004ac
    dcd 0x011f0088
    dcd 0x040004a4
}
