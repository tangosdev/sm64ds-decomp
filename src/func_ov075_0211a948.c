asm void func_ov075_0211a948(void)
{
    stmdb sp!, {r4, lr}
    ldr r3, [r0, #0x90]
    ldr r1, [pc, #0x90]
    ldr r2, [pc, #0x90]
    str r3, [r1]
    ldr r3, [r0, #0x94]
    ldr r1, [pc, #0x88]
    str r3, [r2]
    ldr r4, [r0, #0x98]
    ldr r2, [r0, #0x88]
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
    ldr r1, [r0, #0x98]
    mov r1, r1, lsr #0x1a
    and r1, r1, #7
    cmp r1, #2
    moveq r2, #1
    movne r2, #0
    ldr r1, [r0, #0x8c]
    rsb r0, r2, #4
    mov r3, r1, lsr r0
    ldr r2, [pc, #0x20]
    ldr r1, [pc, #0x20]
    ldr r0, [pc, #0x20]
    str r3, [r2]
    str r1, [r0]
    ldmia sp!, {r4, lr}
    bx lr
    dcd 0x040004c0
    dcd 0x040004c4
    dcd 0x040004a8
    dcd 0x040004ac
    dcd 0x011f0088
    dcd 0x040004a4
}
