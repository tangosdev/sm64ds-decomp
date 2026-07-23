extern void func_02054de8(void);

asm void func_ov006_021027e4(void)
{
    stmdb sp!, {r4-r11, lr}
    sub sp, sp, #4
    mov r10, r1
    mov r9, r3
    mov r8, #0
    str r8, [sp]
    sub r5, r10, #1
    sub r11, r2, #1
L_outer:
    add r1, r11, r8
    and r0, r1, #7
    mov r1, r1, asr #3
    ldr r7, [sp]
    mov r6, r0, lsl #2
    mov r4, r1, lsl #5
L_inner:
    add r10, r5, r7
    bl func_02054de8
    add r1, r4, r10, asr #3
    add r2, r0, r1, lsl #5
    and r0, r10, #7
    add r7, r7, #1
    ldr r1, [r2, r6]
    mov r0, r0, lsl #2
    orr r0, r1, r9, lsl r0
    str r0, [r2, r6]
    cmp r7, #2
    blt L_inner
    add r8, r8, #1
    cmp r8, #2
    blt L_outer
    add sp, sp, #4
    ldmia sp!, {r4-r11, lr}
    bx lr
}
