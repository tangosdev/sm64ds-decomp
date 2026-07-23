asm void func_ov006_0212a654(void)
{
    stmdb sp!, {r4-r11, lr}
    sub sp, sp, #4
    add r1, r0, #0x5000
    mov r2, #1
    strb r2, [r1, #0xfcd]
    mov r1, #0
    mov r2, r1
    mov r3, r1
    mov r7, r1
    mov r8, r1
    mov r6, #0x4000
L_loop:
    ldr r5, [pc, #0xcc]
    add r4, r0, r1, lsl #5
    add r9, r4, r5
    ldrb r5, [r9]
    cmp r5, #0
    beq L_done
    add r5, r4, #0x4000
    ldrb r10, [r5, #0xf3a]
    cmp r10, #1
    addeq r4, r0, #0x5000
    dcd 0x05c48fcd
    beq L_done
    ldrb r5, [r5, #0xf39]
    cmp r5, #0
    addeq r4, r0, #0x5000
    dcd 0x05c47fcd
    beq L_done
    add r5, r0, #0x5000
    ldr r5, [r5, #0xfc8]
    cmp r1, r5
    beq L_done
    ldr r5, [pc, #0x7c]
    add ip, r0, r1, lsl #5
    add r10, r4, r5
    ldr r4, [r10]
    add r11, ip, #0x4000
    add r4, r4, #0x100
    str r4, [r10]
    ldr r4, [r10]
    cmp r4, #0x4000
    ldr r4, [pc, #0x5c]
    dcd 0xa58a6000
    add r5, ip, r4
    ldr lr, [r11, #0xf44]
    ldr r4, [r5]
    ldr r11, [pc, #0x4c]
    add r4, r4, lr
    str r4, [r5]
    ldr r4, [r10]
    ldr r5, [ip, r11]!
    add r4, r5, r4
    str r4, [ip]
    ldr r4, [ip]
    cmp r4, #0x100000
    dcd 0xa5c93000
    addlt r4, r0, #0x5000
    dcd 0xb5c42fcd
L_done:
    add r1, r1, #1
    cmp r1, #0x16
    blt L_loop
    add sp, sp, #4
    ldmia sp!, {r4-r11, lr}
    bx lr
    dcd 0x00004f38
    dcd 0x00004f48
    dcd 0x00004f3c
    dcd 0x00004f40
}
