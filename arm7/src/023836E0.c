// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x023836E0 size 0x90 (runtime 0x037FB578, wram)
// C tried by: AH7 (best C is a near-miss); asm generated from the original bytes.

extern void fn_02383450(void);

asm void fn_023836E0(void) {
    stmdb sp!, {r4, r5, r6, lr}
    mov r6, r1
    mov r5, r3
    mov r4, r0, lsl #4
    ldr r1, =0x38070B8
    strb r2, [r1, r0]
    ldr r1, =0x38070B4
    ldr r1, [r1]
    cmp r1, #0
    ble L_037FB5C4
    mov r1, #1
    mov r1, r1, lsl r0
    ldr r0, =0xFFF5
    ands r0, r1, r0
    beq L_037FB5C4
    mov r0, r2
    ldr r1, [sp, #0x14]
    bl fn_02383450
    mov r2, r0
L_037FB5C4:
    mov r0, r6, lsl #0x18
    orr r1, r0, #0x60000000
    ldr r0, [sp, #0x14]
    orr r0, r1, r0, lsl #16
    orr r0, r0, r5, lsl #8
    orr r1, r2, r0
    add r0, r4, #0x4000000
    str r1, [r0, #0x400]
    ldr r1, [sp, #0x10]
    rsb r1, r1, #0x10000
    add r0, r0, #0x400
    strh r1, [r0, #8]
    ldmia sp!, {r4, r5, r6, lr}
    bx lr
}
