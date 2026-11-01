// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x023A0F58 size 0x288 (runtime 0x027D2ED0, main)
// No C attempt exists yet; asm generated from the original bytes.

extern void fn_02382C58(void);
extern void fn_02382C98(void);
extern void fn_0238D594(void);
extern void fn_0238D7A0(void);
extern void fn_02394E7C(void);
extern void fn_02394FC4(void);

asm void fn_023A0F58(void) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, lr}
    sub sp, sp, #4
    mov r9, r0
    mov r8, r1
    ldr r0, =0x380FFF4
    ldr r0, [r0]
    add r7, r0, #0x4a0
    mvn r6, #0
    mov r1, #0
    mov r2, #0x18
    b L_027D3050
L_027D2EFC:
    mul r5, r1, r2
    add r4, r7, r5
    ldrh r3, [r7, r5]
    cmp r3, #0
    beq L_027D3048
    ldrh ip, [r8, #4]
    ldrh r3, [r4, #8]
    cmp ip, r3
    bne L_027D304C
    ldrh ip, [r8, #2]
    ldrh r3, [r4, #6]
    cmp ip, r3
    bne L_027D304C
    ldrh ip, [r8]
    ldrh r3, [r4, #4]
    cmp ip, r3
    bne L_027D304C
    ldrh ip, [r8, #0xa]
    ldrh r3, [r4, #0xe]
    cmp ip, r3
    bne L_027D304C
    ldrh ip, [r8, #8]
    ldrh r3, [r4, #0xc]
    cmp ip, r3
    bne L_027D304C
    ldrh ip, [r8, #6]
    ldrh r3, [r4, #0xa]
    cmp ip, r3
    bne L_027D304C
    ldrh r3, [r8, #0xc]
    mov r3, r3, lsl #0x10
    mov ip, r3, lsr #0x14
    ldrh r3, [r4, #0x10]
    mov r3, r3, lsl #0x10
    cmp ip, r3, lsr #20
    bne L_027D304C
    ldrh r0, [r9]
    and r0, r0, #0xf0
    mov r1, #0x10
    bl fn_0238D594
    mov r6, r0
    ldrh r0, [r4, #0x10]
    mov r0, r0, lsl #0x1c
    mov r0, r0, lsr #0x1c
    subs r0, r6, r0
    beq L_027D3140
    ands r0, r0, #0x80000000
    bne L_027D3140
    mov r0, r4
    ldr r0, [r0, #0x14]
    add r7, r0, #0x10
    ldrh r2, [r7, #0x10]
    ldrh r0, [r9, #8]
    sub r0, r0, r2
    subs r5, r0, #0x18
    beq L_027D3140
    ands r0, r5, #0x80000000
    bne L_027D3140
    add r0, r9, #0x24
    add r0, r0, r2
    add r1, r7, #0x2c
    add r1, r1, r2
    mov r2, r5
    bl fn_02382C98
    add r1, r7, #0x10
    ldrh r0, [r1]
    add r0, r0, r5
    strh r0, [r1]
    add r2, r4, #0x10
    ldrh r0, [r2]
    bic r1, r0, #0xf
    mov r0, r6, lsl #0x10
    mov r0, r0, lsr #0x10
    and r0, r0, #0xf
    orr r0, r1, r0
    strh r0, [r2]
    ldr r0, =0x380FFF4
    ldr r1, [r0]
    ldr r0, =0x518
    ldr r0, [r1, r0]!
    add r0, r0, r6
    str r0, [r1]
    b L_027D3140
L_027D3048:
    mov r6, r1
L_027D304C:
    add r1, r1, #1
L_027D3050:
    cmp r1, #3
    blo L_027D2EFC
    mvn r1, #0
    cmp r6, r1
    beq L_027D3140
    add r0, r0, #0x14c
    ldr r1, =0x622
    bl fn_02394E7C
    movs r5, r0
    beq L_027D3138
    mov r0, #0x18
    mul r4, r6, r0
    add r6, r7, r4
    mov r0, r8
    add r1, r6, #4
    mov r2, #0x10
    bl fn_02382C98
    mov r0, #5
    strh r0, [r7, r4]
    mov r0, r6
    str r5, [r0, #0x14]
    add r5, r5, #0x10
    mov r0, r9
    add r1, r5, #8
    ldrh r2, [r9, #8]
    add r2, r2, #0xc
    bl fn_02382C98
    ldr r0, =0x380FFF4
    ldr r0, [r0]
    ldr r0, [r0, #0x2d0]
    bl fn_02382C58
    ldrh r0, [r5, #8]
    and r0, r0, #0xf0
    mov r1, #0x10
    bl fn_0238D594
    mov r1, r0
    add r3, r6, #0x10
    ldrh r0, [r3]
    bic r2, r0, #0xf
    mov r0, r1, lsl #0x10
    mov r0, r0, lsr #0x10
    and r0, r0, #0xf
    orr r0, r2, r0
    strh r0, [r3]
    ldr r0, =0x380FFF4
    ldr r2, [r0]
    ldr r0, =0x518
    ldr r0, [r2, r0]!
    add r0, r0, r1
    str r0, [r2]
    ldrh r0, [r9, #8]
    sub r0, r0, #0x18
    strh r0, [r5, #0x10]
    ldrh r0, [r5, #0x10]
    bl fn_0238D7A0
    mov r1, r6
    strh r0, [r1, #2]
    b L_027D3140
L_027D3138:
    mov r0, #4
    bl fn_02394FC4
L_027D3140:
    add sp, sp, #4
    ldmia sp!, {r4, r5, r6, r7, r8, r9, lr}
    bx lr
}
