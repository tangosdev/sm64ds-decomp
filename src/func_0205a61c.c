// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only runtime primitive).
// SDK unaligned block copy - the memcpy twin of func_0205a588's byte-fill (they sit
// adjacent in the ROM). Aligns dst to a halfword with an edge-preserving read-modify-write
// (ldrh; and #0xff; orr src<<8; strh) so no 8-bit store is ever issued, then runs either a
// halfword-shift copy (mismatched src/dst parity) or a word+halfword bulk copy, closing with
// the same edge RMW tail. The hand-tuned alignment ladder and edge RMW are not C-expressible.
// Per asm policy; counts as matched (asm-primitive policy), not a C transcription.
asm void func_0205a61c(void)
{
    cmp r2, #0
    bxeq lr
    tst r1, #1
    beq L40
    ldrh ip, [r1, #-1]
    and ip, ip, #0xff
    tst r0, #1
    ldrneh r3, [r0, #-1]
    movne r3, r3, lsr #8
    ldreqh r3, [r0]
    orr r3, ip, r3, lsl #8
    strh r3, [r1, #-1]
    add r0, r0, #1
    add r1, r1, #1
    subs r2, r2, #1
    bxeq lr
L40:
    eor ip, r1, r0
    tst ip, #1
    beq L94
    bic r0, r0, #1
    ldrh ip, [r0], #2
    mov r3, ip, lsr #8
    subs r2, r2, #2
    blo L78
L60:
    ldrh ip, [r0], #2
    orr ip, r3, ip, lsl #8
    strh ip, [r1], #2
    mov r3, ip, lsr #0x10
    subs r2, r2, #2
    bhs L60
L78:
    tst r2, #1
    bxeq lr
    ldrh ip, [r1]
    and ip, ip, #0xff00
    orr ip, ip, r3
    strh ip, [r1]
    bx lr
L94:
    tst ip, #2
    beq Lc0
    bics r3, r2, #1
    beq L10c
    sub r2, r2, r3
    add ip, r3, r1
Lac:
    ldrh r3, [r0], #2
    strh r3, [r1], #2
    cmp r1, ip
    blo Lac
    b L10c
Lc0:
    cmp r2, #2
    blo L10c
    tst r1, #2
    beq Le0
    ldrh r3, [r0], #2
    strh r3, [r1], #2
    subs r2, r2, #2
    bxeq lr
Le0:
    bics r3, r2, #3
    beq L100
    sub r2, r2, r3
    add ip, r3, r1
Lf0:
    ldr r3, [r0], #4
    str r3, [r1], #4
    cmp r1, ip
    blo Lf0
L100:
    tst r2, #2
    ldrneh r3, [r0], #2
    strneh r3, [r1], #2
L10c:
    tst r2, #1
    bxeq lr
    ldrh r2, [r1]
    ldrh r0, [r0]
    and r2, r2, #0xff00
    and r0, r0, #0xff
    orr r0, r2, r0
    strh r0, [r1]
    bx lr
}
