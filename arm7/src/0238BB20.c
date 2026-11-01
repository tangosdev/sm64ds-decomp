// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x0238BB20 size 0x184 (runtime 0x038039B8, wram)
// No C attempt exists yet; asm generated from the original bytes.

asm void fn_0238BB20(void) {
    mov ip, #0x4000000
    add r1, ip, #0x208
    ldrh r0, [r1]
    tst r0, r0
    bxeq lr
    ldr r3, [ip, #0x210]
    ldr r1, [ip, #0x214]
    ands r2, r1, r3
    bxeq lr
    ldr r0, =0x1DF3FFF
    tst r2, r0
    streq r2, [ip, #0x214]
    bxeq lr
    stmdb sp!, {lr}
    mrs r0, spsr
    stmdb sp!, {r0}
    stmdb sp, {sp, lr} ^
    sub sp, sp, #8
    mov r0, #0xdf
    msr cpsr_c, r0
    ldr r1, =0x3806E60
    ldrh r0, [r1, #4]
    add r0, r0, #1
    strh r0, [r1, #4]
    ldr r1, =0x38098A8
    cmp r0, #1
    moveq r0, sp
    ldreq sp, [r1, #4]
    streq r0, [r1, #4]
    stmdb sp!, {r3}
    ldr r1, =0x3805A64
    ldr r0, [r1]
    tst r0, r2
    strne r0, [ip, #0x214]
    ldrne r0, [r1, #4]
    ldrne r3, =0x3805DF4
    ldrne r0, [r3, r0, lsl #2]
    bne L_03803A94
    mov r3, #1
L_03803A54:
    ldr r0, [r1, r3, lsl #3]
    tst r0, r2
    addeq r3, r3, #1
    beq L_03803A54
    str r0, [ip, #0x214]
    add r0, r1, r3, lsl #3
    ldr r2, [r0, #4]
    ldr r3, =0x3805DF4
    ldr r0, [r3, r2, lsl #2]
    ldr r2, =0x3806E60
    ldrh r3, [r2, #4]
    cmp r3, #1
    ldreq r2, [r1]
    streq r2, [ip, #0x210]
    moveq r2, #0x1f
    msreq cpsr_c, r2
L_03803A94:
    ldr r1, [ip, #0x210]
    stmdb sp!, {r1}
    add lr, pc, #0
    bx r0
    mov r0, #0xdf
    msr cpsr_c, r0
    mov ip, #0x4000000
    ldmia sp!, {r0}
    ldr r1, [ip, #0x210]
    eor r2, r0, r1
    and r1, r2, r1
    and r0, r2, r0
    ldmia sp!, {r3}
    orr r3, r3, r1
    bic r3, r3, r0
    str r3, [ip, #0x210]
    ldr r2, =0x3806E60
    ldr r3, =0x38098A8
    ldrh r0, [r2, #4]
    subs r1, r0, #1
    strh r1, [r2, #4]
    moveq r0, sp
    ldreq sp, [r3, #4]
    streq r0, [r3, #4]
    mov r0, #0xd2
    msr cpsr_c, r0
    ldmia sp, {sp, lr} ^
    mov r0, r0
    add sp, sp, #8
    ldmia sp!, {r0}
    msr spsr_fc, r0
    tst r1, r1
    ldreq r0, =0x37F8490
    addeq lr, pc, #0
    bxeq r0
    ldmia sp!, {pc}
}
