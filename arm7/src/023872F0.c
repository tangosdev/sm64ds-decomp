// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x023872F0 size 0x120 (runtime 0x037FF188, wram)
// No C attempt exists yet; asm generated from the original bytes.

extern void fn_02380FAC(void);
extern void fn_02380FE0(void);
extern void fn_02381050(void);
extern void fn_02382AC4(void);
extern void fn_02382AD8(void);
extern void fn_02383024(void);

asm void fn_023872F0(void) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    sub sp, sp, #4
    ldr r10, =0x38087A0
    add r5, r10, #0x40
    mov r8, #0
    mov r4, #0xb
    mov r6, #1
    add r7, r10, #0x38
L_037FF1A8:
    mov r9, r8
    bl fn_02382AC4
    mov r11, r0
L_037FF1B4:
    ldr r0, [r10, #0x38]
    ands r0, r0, #4
    bne L_037FF1EC
    ldr r0, [r10, #0x38]
    ands r0, r0, #0x10
    beq L_037FF1F8
    ldr r0, [r7]
    orr r0, r0, #4
    str r0, [r7]
    ldr r0, [r7]
    bic r0, r0, #0x10
    str r0, [r7]
    mov r9, r6
    b L_037FF208
L_037FF1EC:
    ldr r0, [r10, #0x38]
    ands r0, r0, #8
    bne L_037FF208
L_037FF1F8:
    str r5, [r10, #0xb8]
    mov r0, r8
    bl fn_02381050
    b L_037FF1B4
L_037FF208:
    mov r0, r11
    bl fn_02382AD8
    cmp r9, #0
    beq L_037FF28C
    ldr r0, [r10]
    str r8, [r0]
    mov r0, r10
    ldr r2, [r10, #4]
    ldr r1, =0x3805A30
    ldr r1, [r1, r2, lsl #2]
    mov lr, pc
    bx r1
L_037FF238:
    mov r0, r4
    mov r1, r6
    mov r2, r6
    bl fn_02383024
    cmp r0, #0
    blt L_037FF238
    bl fn_02382AC4
    mov r9, r0
    ldr r0, [r7]
    bic r0, r0, #0xc
    str r0, [r7]
    add r0, r10, #0xbc
    bl fn_02380FE0
    ldr r0, [r10, #0x38]
    ands r0, r0, #0x10
    beq L_037FF280
    mov r0, r5
    bl fn_02380FAC
L_037FF280:
    mov r0, r9
    bl fn_02382AD8
    b L_037FF1A8
L_037FF28C:
    mov r0, r10
    ldr r1, [r10, #0x34]
    mov lr, pc
    bx r1
    b L_037FF1A8
}
