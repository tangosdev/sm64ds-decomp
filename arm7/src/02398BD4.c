// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02398BD4 size 0x2B4 (runtime 0x027CAB4C, main)
// C tried by: AH46 (best C is a near-miss); asm generated from the original bytes.

extern void fn_0239477C(void);
extern void fn_02394FC4(void);
extern void fn_0239852C(void);

asm void fn_02398BD4(void) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    sub sp, sp, #4
    mov r1, #1
    ldr r0, =0x4808010
    strh r1, [r0]
    ldr r4, =0x48080F8
    ldr r5, =0x380FFF4
    ldr r11, =0x4808054
    ldr r8, =0x4804000
    ldr r9, =0x4805F60
L_027CAB74:
    ldr r1, [r5]
    add r0, r1, #0x400
    ldrh r2, [r0, #0x9c]
    ldrh lr, [r11]
    cmp r2, lr
    beq L_027CAD5C
    ldrh r3, [r4]
    ldr r0, =0x48080FA
    ldrh r0, [r0]
    ldrh r7, [r4]
    ldr r6, =0x48080FA
    ldrh r6, [r6]
    cmp r3, r7
    movhi r0, r7, lsr #4
    orrhi ip, r0, r6, lsl #12
    movls r3, r3, lsr #4
    orrls ip, r3, r0, lsl #12
    add r0, r8, r2, lsl #1
    add r7, r0, #2
    cmp r7, r9
    addhs r3, r1, #0x300
    ldrhsh r3, [r3, #0x96]
    subhs r7, r7, r3
    add r6, r7, #2
    cmp r6, r9
    addhs r3, r1, #0x300
    ldrhsh r3, [r3, #0x96]
    subhs r6, r6, r3
    add r3, r6, #4
    cmp r3, r9
    addhs r1, r1, #0x300
    ldrhsh r1, [r1, #0x96]
    subhs r3, r3, r1
    ldrh r1, [r0]
    ldrh r10, [r7]
    mov r10, r10, lsl #1
    and r10, r10, #0x400
    orr r1, r1, r10
    strh r1, [r0]
    strh ip, [r6]
    ldrh ip, [r3]
    add r1, ip, r2, lsl #1
    add r1, r1, #0xf
    mov r1, r1, lsr #2
    mov r6, r1, lsl #1
    cmp r6, #0xfb0
    ldrhs r1, [r5]
    addhs r1, r1, #0x300
    ldrhsh r1, [r1, #0x96]
    subhs r6, r6, r1, lsr #1
    ldr r1, =0x92C
    cmp ip, r1
    bls L_027CAC6C
    ldr r1, =0xFFFF
    strh r1, [r0]
    mov r6, lr
    ldr r1, [r5]
    add r2, r1, #0x3b0
    ldrh r1, [r2]
    add r1, r1, #1
    strh r1, [r2]
    b L_027CAD14
L_027CAC6C:
    ldr r2, [r5]
    add r1, r2, #0x600
    ldrh r1, [r1, #0x1c]
    ands r1, r1, #1
    beq L_027CAD14
    cmp r6, lr
    beq L_027CAD14
    add r1, r8, r6, lsl #1
    ldrh r3, [r1]
    ldr r10, =0x4805F5A
    cmp r1, r10
    ldrloh r1, [r1, #6]
    andlo r1, r1, #0xff
    movlo r1, r1, lsl #0x10
    movlo r1, r1, lsr #0x10
    addhs r10, r2, #0x300
    ldrhsh r10, [r10, #0x96]
    subhs r1, r1, r10
    ldrhsh r1, [r1, #6]
    ands r3, r3, #0x7c00
    bne L_027CACDC
    cmp r1, #0xa
    beq L_027CACD0
    cmp r1, #0x14
    bne L_027CACDC
L_027CACD0:
    ldr r1, =0xFFF
    cmp ip, r1
    bls L_027CAD14
L_027CACDC:
    add r2, r2, #0x3b0
    ldrh r1, [r2]
    add r1, r1, #1
    strh r1, [r2]
    ldr r1, =0xFFFF
    strh r1, [r0]
    mov r0, lr, lsl #0x10
    mov r1, r0, lsr #0x10
    ldr r0, =0x380FFF4
    ldr r0, [r0]
    add r0, r0, #0x400
    strh r1, [r0, #0x9c]
    strh r1, [r7]
    b L_027CAD5C
L_027CAD14:
    ldrh r0, [r0]
    and r0, r0, #0xf
    cmp r0, #0xc
    bne L_027CAD40
    bl fn_0239852C
    cmp r0, #0
    ldreq r0, [r5]
    addeq r1, r0, #0x560
    ldreq r0, [r1]
    addeq r0, r0, #1
    streq r0, [r1]
L_027CAD40:
    mov r0, r6, lsl #0x10
    mov r1, r0, lsr #0x10
    ldr r0, [r5]
    add r0, r0, #0x400
    strh r1, [r0, #0x9c]
    strh r1, [r7]
    b L_027CAB74
L_027CAD5C:
    ldr r0, =0x380FFF4
    ldr r0, [r0]
    add r0, r0, #0x600
    ldrh r0, [r0, #0x1c]
    ands r0, r0, #1
    beq L_027CADA0
    ldr r0, =0x4808054
    ldrh r4, [r0]
    bl fn_0239852C
    cmp r0, #0
    beq L_027CADA0
    ldr r0, =0x4808054
    ldrh r0, [r0]
    cmp r4, r0
    bne L_027CADA0
    mov r0, #0x20
    bl fn_02394FC4
L_027CADA0:
    ldr r0, =0x480805A
    ldrh r1, [r0]
    ldr r0, =0x4808054
    ldrh r0, [r0]
    cmp r1, r0
    beq L_027CADC4
    mov r0, #0
    mov r1, #0xf
    bl fn_0239477C
L_027CADC4:
    add sp, sp, #4
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}
