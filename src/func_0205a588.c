// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only runtime primitive).
// SDK byte-fill (memset): aligns to halfword then word with read-modify-write at the
// unaligned head/tail (ldrh; and #0xff00; orr; strh to preserve the adjacent byte),
// replicates the fill byte across the word, and runs a str-writeback word loop. The
// edge-preserving RMW and the hand-tuned alignment ladder are not C-expressible. Per asm policy.
asm void func_0205a588(void)
{
    cmp r2, #0
    bxeq lr
    tst r0, #1
    beq L2c
    ldrh ip, [r0, #-1]
    and ip, ip, #0xff
    orr r3, ip, r1, lsl #8
    strh r3, [r0, #-1]
    add r0, r0, #1
    subs r2, r2, #1
    bxeq lr
L2c:
    cmp r2, #2
    blo L74
    orr r1, r1, r1, lsl #8
    tst r0, #2
    beq L4c
    strh r1, [r0], #2
    subs r2, r2, #2
    bxeq lr
L4c:
    orr r1, r1, r1, lsl #16
    bics r3, r2, #3
    beq L6c
    sub r2, r2, r3
    add ip, r3, r0
L60:
    str r1, [r0], #4
    cmp r0, ip
    blo L60
L6c:
    tst r2, #2
    strneh r1, [r0], #2
L74:
    tst r2, #1
    bxeq lr
    ldrh r3, [r0]
    and r3, r3, #0xff00
    and r1, r1, #0xff
    orr r1, r1, r3
    strh r1, [r0]
    bx lr
}
