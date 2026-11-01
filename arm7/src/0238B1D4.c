// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x0238B1D4 size 0x214 (runtime 0x0380306C, wram)
// No C attempt exists yet; asm generated from the original bytes.

extern void fn_02382AC4(void);
extern void fn_02382AD8(void);
extern void fn_02388288(void);
extern void fn_023882B0(void);
extern void fn_02388384(void);
extern void fn_023883C8(void);
extern void fn_023883E0(void);
extern void fn_023883FC(void);
extern void fn_0238B900(void);
extern void fn_0238BA10(void);

asm void fn_0238B1D4(void) {
    stmdb sp!, {r4, lr}
    bl fn_02388288
    cmp r0, #0
    bne L_03803110
    bl fn_02382AC4
    mov r4, r0
    mov r0, #2
    bl fn_023883E0
    cmp r0, #0
    bne L_038030A0
    mov r0, r4
    bl fn_02382AD8
    b L_03803110
L_038030A0:
    mov r0, #2
    bl fn_023883C8
    mov r0, r4
    bl fn_02382AD8
    ldr r0, =0x380986C
    ldrh r0, [r0, #0x26]
    and r0, r0, #1
    cmp r0, #1
    bne L_038030E8
    bl fn_0238B900
    ldr r1, =0x380986C
    strh r0, [r1, #0x38]
    ldrh r0, [r1, #0x26]
    ands r0, r0, #2
    ldrneh r0, [r1, #0x38]
    eorne r0, r0, #0x8000
    strneh r0, [r1, #0x38]
    b L_03803108
L_038030E8:
    bl fn_0238BA10
    ldr r1, =0x380986C
    strh r0, [r1, #0x3a]
    ldrh r0, [r1, #0x26]
    ands r0, r0, #2
    ldrneh r0, [r1, #0x3a]
    eorne r0, r0, #0x80
    strneh r0, [r1, #0x3a]
L_03803108:
    mov r0, #2
    bl fn_02388384
L_03803110:
    ldr r0, =0x380986C
    ldrh r1, [r0, #0x26]
    and r1, r1, #1
    cmp r1, #1
    bne L_03803168
    ldrh r3, [r0, #0x38]
    ldr r2, [r0, #0x28]
    ldr r1, [r0, #0x2c]
    bic r1, r1, #1
    strh r3, [r2, r1]
    ldr r1, [r0, #0x2c]
    add r1, r1, #2
    str r1, [r0, #0x2c]
    ldrh r2, [r0, #0x38]
    ldr r1, =0x27FFF94
    strh r2, [r1]
    ldr r1, [r0, #0x28]
    ldr r0, [r0, #0x2c]
    add r1, r1, r0
    ldr r0, =0x27FFF90
    str r1, [r0]
    b L_038031D8
L_03803168:
    ldr ip, [r0, #0x2c]
    ands r1, ip, #1
    ldreqh r1, [r0, #0x3a]
    andeq r1, r1, #0xff
    streqh r1, [r0, #0x38]
    beq L_038031C8
    ldrh r2, [r0, #0x38]
    ldrh r1, [r0, #0x3a]
    mov r1, r1, lsl #8
    and r1, r1, #0xff00
    orr r1, r2, r1
    strh r1, [r0, #0x38]
    ldrh r3, [r0, #0x38]
    ldr r2, [r0, #0x28]
    bic r1, ip, #1
    strh r3, [r2, r1]
    ldrh r2, [r0, #0x38]
    ldr r1, =0x27FFF94
    strh r2, [r1]
    ldr r1, [r0, #0x28]
    ldr r0, [r0, #0x2c]
    add r1, r1, r0
    ldr r0, =0x27FFF90
    str r1, [r0]
L_038031C8:
    ldr r0, =0x380986C
    ldr r1, [r0, #0x2c]
    add r1, r1, #1
    str r1, [r0, #0x2c]
L_038031D8:
    ldr r0, =0x380986C
    ldr r2, [r0, #0x2c]
    ldr r1, [r0, #0x30]
    cmp r2, r1
    blo L_03803254
    mov r2, #0
    str r2, [r0, #0x2c]
    ldrh r0, [r0, #0x24]
    and r0, r0, #0x10
    cmp r0, #0x10
    bne L_03803214
    mov r0, #0x51
    mov r1, r2
    bl fn_023883FC
    b L_03803254
L_03803214:
    mov r0, #2
    mov r1, #0x42
    bl fn_023882B0
    cmp r0, #0
    bne L_03803238
    mov r0, #0x51
    mov r1, #4
    bl fn_023883FC
    b L_03803254
L_03803238:
    mov r1, #4
    ldr r0, =0x380986C
    str r1, [r0, #0x20]
    ldr r1, =0x400010E
    ldrh r0, [r1]
    bic r0, r0, #0x80
    strh r0, [r1]
L_03803254:
    ldr r1, =0x380FFF8
    ldr r0, [r1]
    orr r0, r0, #0x40
    str r0, [r1]
    ldmia sp!, {r4, lr}
    bx lr
}
