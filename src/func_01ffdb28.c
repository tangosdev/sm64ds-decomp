// HAND-ASM PRIMITIVE: byte-faithful asm-block match.
extern void func_02059d98(void);

asm void func_01ffdb28(void) {
    mov     r12, #0x4000000
    str     r12, [r12, #0x208]
    ldr     r1, [pc, #0x88]
    add     r1, r1, #0x3fc0
    add     r1, r1, #0x3c
    mov     r0, #0
    str     r0, [r1]
    ldr     r1, [pc, #0x78]
_01ffdb48:
    ldrh    r0, [r1]
    and     r0, r0, #0xf
    cmp     r0, #1
    bne     _01ffdb48
    mov     r0, #0x100
    strh    r0, [r1]
    ldr     r0, [pc, #0x60]
    ldr     r12, [r0]
    ldr     r0, [pc, #0x5c]
    mov     r1, #0x80
    bl      func_02059d98
    ldr     r0, [pc, #0x4c]
    str     r12, [r0]
    ldr     r0, [pc, #0x4c]
    mov     r1, #0x80
    bl      func_02059d98
    ldr     r1, [pc, #0x34]
_01ffdb8c:
    ldrh    r0, [r1]
    and     r0, r0, #0xf
    cmp     r0, #1
    beq     _01ffdb8c
    mov     r0, #0
    strh    r0, [r1]
    ldr     r3, [pc, #0x28]
    ldr     r12, [r3, #0x24]
    mov     lr, r12
    ldr     r11, [pc, #8]
    ldmia   r11, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10}
    mov     r11, #0
    bx      r12
    dcd     0x023c0000
    dcd     0x04000180
    dcd     0x027ffd9c
    dcd     0x027ffd80
    dcd     0x027fff80
    dcd     0x027ffe00
}
