// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x0238B11C size 0xB8 (runtime 0x03802FB4, wram)
// No C attempt exists yet; asm generated from the original bytes.

asm void fn_0238B11C(void) {
    mov ip, #0x4000000
    add ip, ip, #0x138
L_03802FBC:
    stmdb sp!, {r0, r1}
    ldrh r0, [ip]
    bic r0, r0, #0x77
    orr r0, r0, #0x64
    mov r2, #0
    mov r1, #0
L_03802FD4:
    bic r0, r0, #3
    orr r0, r0, #0
    strh r0, [ip]
    mov r3, #9
L_03802FE4:
    subs r3, r3, #1
    bne L_03802FE4
    ldrh r0, [ip]
    and r3, r0, #1
    cmp r3, #1
    moveq r3, #0x80
    movne r3, #0
    orr r2, r3, r2, lsr #1
    bic r0, r0, #2
    orr r0, r0, #2
    strh r0, [ip]
    mov r3, #9
L_03803014:
    subs r3, r3, #1
    bne L_03803014
    add r1, r1, #1
    cmp r1, #8
    bne L_03802FD4
    ldmia sp!, {r0, r1}
    tst r0, #1
    beq L_0380304C
    ldrh r3, [r0, #-1]
    bic r3, r3, #0xff00
    mov r2, r2, lsl #8
    orr r3, r2, r3
    strh r3, [r0, #-1]
    b L_0380305C
L_0380304C:
    ldrh r3, [r0]
    bic r3, r3, #0xff
    orr r3, r3, r2
    strh r3, [r0]
L_0380305C:
    add r0, r0, #1
    subs r1, r1, #1
    bne L_03802FBC
    bx lr
}
