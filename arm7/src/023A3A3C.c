// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x023A3A3C size 0x270 (runtime 0x027D59B4, main)
// No C attempt exists yet; asm generated from the original bytes.

extern void fn_0239AE84(void);

asm void fn_023A3A3C(void) {
    stmdb sp!, {r4, r5, lr}
    sub sp, sp, #4
    ldr r1, =0x380FFF4
    ldr r5, [r1]
    add r1, r5, #0x308
    add r2, r5, #0x300
    ldrh r2, [r2, #0x10]
    cmp r2, #0x40
    movne r0, #1
    bne L_027D5BFC
    ldrh r4, [r0, #0x2a]
    add r2, r5, #0x400
    ldrh r3, [r2, #0x98]
    cmp r3, r4
    bne L_027D5A0C
    ldr r0, =0x534
    add r1, r5, r0
    ldr r0, [r1]
    add r0, r0, #1
    str r0, [r1]
    mov r0, #1
    b L_027D5BFC
L_027D5A0C:
    strh r4, [r2, #0x98]
    ldrh r3, [r0, #0x22]
    ldrh r2, [r1, #0x5c]
    cmp r3, r2
    bne L_027D5A44
    ldrh r3, [r0, #0x20]
    ldrh r2, [r1, #0x5a]
    cmp r3, r2
    bne L_027D5A44
    ldrh r3, [r0, #0x1e]
    ldrh r2, [r1, #0x58]
    cmp r3, r2
    moveq r2, #1
    beq L_027D5A48
L_027D5A44:
    mov r2, #0
L_027D5A48:
    cmp r2, #0
    beq L_027D5A90
    ldrh r3, [r0, #0x28]
    ldrh r2, [r1, #0x7a]
    cmp r3, r2
    bne L_027D5A84
    ldrh r3, [r0, #0x26]
    ldrh r2, [r1, #0x78]
    cmp r3, r2
    bne L_027D5A84
    ldrh r2, [r0, #0x24]
    ldrh r1, [r1, #0x76]
    cmp r2, r1
    moveq r1, #1
    beq L_027D5A88
L_027D5A84:
    mov r1, #0
L_027D5A88:
    cmp r1, #0
    bne L_027D5A98
L_027D5A90:
    mov r0, #1
    b L_027D5BFC
L_027D5A98:
    ldr r1, =0x380FFF4
    ldr r3, [r1]
    mov r2, #1
    add r1, r3, #0x300
    ldrh r1, [r1, #0x66]
    mov r1, r2, lsl r1
    mov r1, r1, lsl #0x10
    mov r2, r1, lsr #0x10
    ldrh r1, [r0, #0x2e]
    ands r1, r2, r1
    moveq r2, #0
    addeq r1, r3, #0x400
    streqh r2, [r1, #0x9e]
    movne r2, #0x2000
    addne r1, r3, #0x400
    strneh r2, [r1, #0x9e]
    ldr r1, =0x4808098
    ldrh r2, [r1]
    ands r1, r2, #0x8000
    beq L_027D5B20
    ldr r1, =0x7FFF
    and r1, r2, r1
    mov r1, r1, lsl #1
    add r1, r1, #0x4800000
    add r1, r1, #0x4000
    ldrh r1, [r1, #4]
    cmp r1, #0
    beq L_027D5B20
    ldr r1, =0x380FFF4
    ldr r2, [r1]
    ldr r1, =0x49E
    ldrh r1, [r2, r1]!
    orr r1, r1, #0x4000
    strh r1, [r2]
L_027D5B20:
    ldr r1, =0x380FFF4
    ldr r2, [r1]
    add r1, r2, #0x300
    ldrh r3, [r1, #0x84]
    mov r1, #0x1c
    ldr r2, [r2, #0x2e0]
    mla r2, r3, r1, r2
    ldrh r1, [r2, #0x1a]
    strh r1, [r2, #0x18]
    ldrh r1, [r0, #0x10]
    sub r1, r1, #0x1c
    strh r1, [r0, #6]
    sub r1, r0, #0x10
    ldr r2, =0x182
    strh r2, [r1, #0xc]
    ldrh r0, [r0, #6]
    add r0, r0, #0x31
    mov r0, r0, lsr #1
    strh r0, [r1, #0xe]
    ldrh r2, [r1, #0x3e]
    mov ip, #0
    b L_027D5B88
L_027D5B78:
    ands r0, r2, #1
    addne ip, ip, #1
    mov r0, r2, lsl #0xf
    mov r2, r0, lsr #0x10
L_027D5B88:
    cmp r2, #0
    bne L_027D5B78
    add r3, r1, #0x18
    ldrh r2, [r3]
    ldr r0, =0x4808094
    ldrh r0, [r0]
    and r5, r0, #0x8000
    ldr r0, =0x380FFF4
    ldr r4, [r0]
    add lr, r4, #0x400
    ldrh r4, [lr, #0x9e]
    ldr lr, =0x4808098
    ldrh lr, [lr]
    and lr, lr, #0x8000
    orr r4, r4, lr, asr #4
    orr r4, r4, r5, asr #3
    orr r2, r2, r4
    strh r2, [r3]
    ldrh r3, [r1, #0x1c]
    ldrh r2, [r1, #0x3c]
    add r2, r2, #0xa
    mul r2, ip, r2
    add r2, r2, #0xfc
    add r2, r3, r2, lsr #4
    strh r2, [r1, #0x1a]
    ldr r0, [r0]
    add r0, r0, #0x14c
    bl fn_0239AE84
    mov r0, #0
L_027D5BFC:
    add sp, sp, #4
    ldmia sp!, {r4, r5, lr}
    bx lr
}
