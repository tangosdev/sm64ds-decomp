// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x0239A00C size 0x224 (runtime 0x027CBF84, main)
// C tried by: the AC C-only redo round; no C attempt reproduced the bytes (see the notes below).
/* Hand-asm: two `bx r2` indirect handler calls with `mov lr,pc` plus three
   shared JUMPOUT tails — unreachable from C.
   Pool: 0x380FFF4, 0x48080A0, 0x4808032, 0x3B6. */
extern int *dat_0380FFF4;

asm void fn_0239A00C(void)
{
    stmfd sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    sub sp, sp, #4
    ldr r4, =dat_0380FFF4
    ldr r0, [r4]
    add r6, r0, #0x3E4
    mov r5, #2
    mov r10, #0
    mov r9, #0x8000
    mov r11, #3
    str r5, [sp]
    mov r8, r5
    mov r7, #1
loc_B4:
    ldr r0, [r4]
    add r1, r0, #0x3E4
    mov r0, #0x14
    mla r3, r5, r0, r1
    ldr r0, =0x48080A0
    add r1, r0, r5, lsl #2
    ldrh r0, [r1]
    ands r0, r0, #0x8000
    bne loc_184
    ldrh r0, [r3]
    cmp r0, #0
    beq loc_184
    ldr r2, [r3, #0xC]
    cmp r2, #0
    beq loc_160
    ldr r0, [r3, #8]
    ldrh r0, [r0]
    strh r0, [r2, #8]
    ldr r0, [r3, #8]
    ldrh r2, [r0, #0xC]
    ldr r0, [r3, #0xC]
    strh r2, [r0, #0x14]
    ldr r2, [r3, #8]
    ldrh r0, [r2, #0xC]
    ands r0, r0, #0x4000
    beq loc_128
    ldr r0, [r4]
    add r12, r0, #0x600
    ldrh r12, [r12, #0x1C]
    ands r12, r12, #8
    beq loc_104
    add lr, r2, #0xC
    ldrh r12, [r2, #0xA]
    add r12, lr, r12
    sub r12, r12, #7
    bic lr, r12, #1
    ldrh r12, [lr]
    cmp r12, #0
    bne loc_104
    ldrh r12, [lr, #2]
    cmp r12, #0
    bne loc_104
    ldr r2, [r0, #0x3A4]
    ldr r0, [r3, #0xC]
    ldrh r0, [r0, #4]
    sub r0, r2, r0
    mov r0, r0, lsl #16
    mov r2, r0, lsr #16
    ldr r0, =0x4808032
    strh r10, [r0]
    strh r9, [r0]
    ldr r12, [r4]
    ldr r0, =0x3B6
    add r0, r12, r0
    ldrh r12, [r0]
    add r12, r12, #1
    strh r12, [r0]
    ldr r0, [r4]
    add r0, r0, #0x300
    ldrh r0, [r0, #0x88]
    cmp r2, r0
    bls loc_EC
    ldr r0, [r3, #0xC]
    strh r8, [r0, #8]
    add r1, r3, #4
    ldrh r0, [r1]
    add r0, r0, #1
    strh r0, [r1]
    add r1, r6, #0xB0
    ldrh r0, [r1]
    add r0, r0, #1
    strh r0, [r1]
    ldr r0, [r3, #0xC]
    mov r1, r7
    ldr r2, [r3, #0x10]
    mov lr, pc
    bx r2
    b loc_184
loc_EC:
    ldr r0, [r3, #0xC]
    strh r10, [r0, #0xC]
    ldrh r0, [r1]
    orr r0, r0, #0x8000
    strh r0, [r1]
    b loc_184
loc_104:
    ldr r0, [r3, #0xC]
    add r1, r0, #0xC
    ldrh r0, [r1]
    ldrh r2, [r2, #4]
    and r2, r2, #0xFF
    mov r2, r2, lsl #16
    add r0, r0, r2, lsr #16
    strh r0, [r1]
    b loc_138
loc_128:
    ldrh r0, [r2, #4]
    and r1, r0, #0xFF
    ldr r0, [r3, #0xC]
    strh r1, [r0, #0xC]
loc_138:
    add r1, r3, #4
    ldrh r0, [r1]
    add r0, r0, #1
    strh r0, [r1]
    ldr r0, [r3, #0xC]
    ldr r1, [sp]
    ldr r2, [r3, #0x10]
    mov lr, pc
    bx r2
    b loc_184
loc_160:
    add r1, r3, #4
    ldrh r0, [r1]
    add r0, r0, #1
    strh r0, [r1]
    ldr r0, [r3, #8]
    mov r1, r11
    ldr r2, [r3, #0x10]
    mov lr, pc
    bx r2
loc_184:
    subs r5, r5, #1
    bpl loc_B4
    add sp, sp, #4
    ldmfd sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}
