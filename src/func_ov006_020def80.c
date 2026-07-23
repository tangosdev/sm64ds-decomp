asm void func_ov006_020def80(void)
{
    add r2, r0, r1
    add r2, r2, #0x5000
    ldrb r3, [r2, #0x465]
    cmp r3, #0
    bne L_nonzero
    ldrb r2, [r2, #0x462]
    cmp r2, #0
    addne r2, r0, r1, lsl #2
    addne r2, r2, #0x5000
    movne r3, #4
    strne r3, [r2, #0x434]
    addeq r2, r0, r1, lsl #2
    addeq r2, r2, #0x5000
    moveq r3, #1
    streq r3, [r2, #0x434]
    b L_done
L_nonzero:
    ldrb r2, [r2, #0x462]
    cmp r2, #0
    beq L_zero
    add r2, r0, r1, lsl #2
    add r2, r2, #0x5000
    mov r3, #5
    str r3, [r2, #0x434]
    add r2, r0, #0x5000
    ldrb r3, [r2, #0x46d]
    cmp r3, #0xff
    bne L_done
    ldrb r3, [r2, #0x469]
    cmp r3, #1
    dcd 0x15c2146d
    b L_done
L_zero:
    add r2, r0, r1, lsl #2
    add r2, r2, #0x5000
    mov r3, #2
    str r3, [r2, #0x434]
L_done:
    add r0, r0, r1, lsl #2
    add r0, r0, #0x5000
    mov r1, #0
    str r1, [r0, #0x440]
    str r1, [r0, #0x44c]
    bx lr
}
