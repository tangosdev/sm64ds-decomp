// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
asm int func_0206c8b4(int a, int b, int c, int d) {
    stmdb sp!, {r0-r3}
    add r0, sp, #0
    add r1, r0, #7
    ands r0, r1, #1
    ldrneh r0, [r1, #-1]
    andne r0, r0, #0xff00
    movne r2, r0, asr #8
    ldreqh r0, [r1]
    andeq r2, r0, #0xff
    add r0, sp, #0
    add r1, r0, #6
    ands r0, r1, #1
    ldrneh r0, [r1, #-1]
    andne r0, r0, #0xff00
    movne r1, r0, asr #8
    ldreqh r0, [r1]
    andeq r1, r0, #0xff
    and r0, r2, #0x7f
    mov r0, r0, lsl #4
    add r0, r0, r1, asr #4
    add sp, sp, #0x10
    bx lr
}
