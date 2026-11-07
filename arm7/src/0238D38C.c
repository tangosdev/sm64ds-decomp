// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: compiler 64-bit divide/modulo runtime (four entries sharing two bodies).
// @ 0x0238D38C size 0x208 (runtime 0x03805224, wram)
/* storage 0x0238D38C runtime 0x03805224 size 0x208 wram ARM.
 * HAND-ASM COMPILER-RUNTIME HELPERS (not C): four entry points sharing two
 * bodies, i.e. the MSL signed/unsigned 64-bit divide+modulo family. One C
 * function has one entry; no C shape emits this. Transcribed word-for-word
 * from the ROM bytes (capstone cross-checked); all branch targets are inside
 * the 0x208 range. Interior entries: +0x0C (unsigned-divide path),
 * +0x48 (signed-modulo path, fleet absolute fn_0238D3E4 via AM13),
 * +0x58 (unsigned-modulo path). External refs: bl to interior
 * fn_0238D7A8 (+8 inside AR20's swapped fn_0238D7A0: zero-check skipped) and
 * bl fn_0238D594 (absolute, unenrolled). The unreachable
 * `lsr r0, r1, r7; mov r1, #0` pair at +0x1C8 is transcribed as-is. */

extern int fn_0238D7A8(int a1, int a2);
extern int fn_0238D594(int a1, int a2);

asm void fn_0238D38C(void) {
    stmdb sp!, {r4, r5, r6, r7, r11, r12, r14}
    mov r4, #0
    b L05238
L05230:
    stmdb sp!, {r4, r5, r6, r7, r11, r12, r14}
    mov r4, #1
L05238:
    orrs r5, r3, r2
    bne L05248
    ldmia sp!, {r4, r5, r6, r7, r11, r12, r14}
    bx lr
L05248:
    orrs r5, r1, r3
    bne L052F0
    mov r1, r2
    bl fn_0238D7A8
    cmp r4, #0
    movne r0, r1
    mov r1, #0
    ldmia sp!, {r4, r5, r6, r7, r11, r12, r14}
    bx lr
L0526C:
    stmdb sp!, {r4, r5, r6, r7, r11, r12, r14}
    mov r4, r1
    orr r4, r4, #1
    b L0528C
L0527C:
    stmdb sp!, {r4, r5, r6, r7, r11, r12, r14}
    eor r4, r1, r3
    mov r4, r4, asr #1
    mov r4, r4, lsl #1
L0528C:
    orrs r5, r3, r2
    bne L0529C
    ldmia sp!, {r4, r5, r6, r7, r11, r12, r14}
    bx lr
L0529C:
    mov r5, r0, lsr #31
    add r5, r5, r1
    mov r6, r2, lsr #31
    add r6, r6, r3
    orrs r6, r5, r6
    bne L052D0
    mov r1, r2
    bl fn_0238D594
    ands r4, r4, #1
    movne r0, r1
    mov r1, r0, asr #31
    ldmia sp!, {r4, r5, r6, r7, r11, r12, r14}
    bx lr
L052D0:
    cmp r1, #0
    bge L052E0
    rsbs r0, r0, #0
    rsc r1, r1, #0
L052E0:
    cmp r3, #0
    bge L052F0
    rsbs r2, r2, #0
    rsc r3, r3, #0
L052F0:
    orrs r5, r1, r0
    beq L05414
    mov r5, #0
    mov r6, #1
    cmp r3, #0
    bmi L0531C
L05308:
    add r5, r5, #1
    adds r2, r2, r2
    adcs r3, r3, r3
    bpl L05308
    add r6, r6, r5
L0531C:
    cmp r1, #0
    blt L0533C
L05324:
    cmp r6, #1
    beq L0533C
    sub r6, r6, #1
    adds r0, r0, r0
    adcs r1, r1, r1
    bpl L05324
L0533C:
    mov r7, #0
    mov r12, #0
    mov r11, #0
    b L05364
L0534C:
    orr r12, r12, #1
    subs r6, r6, #1
    beq L053BC
    adds r0, r0, r0
    adcs r1, r1, r1
    adcs r7, r7, r7
L05364:
    subs r0, r0, r2
    sbcs r1, r1, r3
    sbcs r7, r7, #0
    adds r12, r12, r12
    adc r11, r11, r11
    cmp r7, #0
    bge L0534C
L05380:
    subs r6, r6, #1
    beq L053B4
    adds r0, r0, r0
    adcs r1, r1, r1
    adc r7, r7, r7
    adds r0, r0, r2
    adcs r1, r1, r3
    adc r7, r7, #0
    adds r12, r12, r12
    adc r11, r11, r11
    cmp r7, #0
    bge L0534C
    b L05380
L053B4:
    adds r0, r0, r2
    adc r1, r1, r3
L053BC:
    ands r7, r4, #1
    moveq r0, r12
    moveq r1, r11
    beq L053F4
    subs r7, r5, #0x20
    movge r0, r1, lsr r7
    bge L05418
    rsb r7, r5, #0x20
    mov r0, r0, lsr r5
    orr r0, r0, r1, lsl r7
    mov r1, r1, lsr r5
    b L053F4
    mov r0, r1, lsr r7
    mov r1, #0
L053F4:
    cmp r4, #0
    blt L05404
    ldmia sp!, {r4, r5, r6, r7, r11, r12, r14}
    bx lr
L05404:
    rsbs r0, r0, #0
    rsc r1, r1, #0
    ldmia sp!, {r4, r5, r6, r7, r11, r12, r14}
    bx lr
L05414:
    mov r0, #0
L05418:
    mov r1, #0
    cmp r4, #0
    blt L05404
    ldmia sp!, {r4, r5, r6, r7, r11, r12, r14}
    bx lr
}
