// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only runtime primitive).
// SDK byte-fill (memset): replicates the fill byte across a word and runs an alignment
// ladder (strb head to word-align, str-writeback word loop, strb tail). The hand-tuned
// alignment ladder is not reproducible from C under our flags. Per asm policy.
asm void func_0206e330(void *dst, unsigned int val, unsigned int n)
{
    cmp     r2, #0x20
    and     ip, r1, #0xff
    blo     L90
    rsb     r1, r0, #0
    ands    r3, r1, #3
    beq     L2c
    sub     r2, r2, r3
    and     r1, ip, #0xff
L20:
    strb    r1, [r0], #1
    subs    r3, r3, #1
    bne     L20
L2c:
    cmp     ip, #0
    movne   r1, ip, lsl #0x18
    orrne   r1, r1, ip, lsl #16
    orrne   r1, r1, ip, lsl #8
    orrne   ip, ip, r1
    movs    r3, r2, lsr #5
    beq     L74
L48:
    str     ip, [r0]
    str     ip, [r0, #4]
    str     ip, [r0, #8]
    str     ip, [r0, #0xc]
    str     ip, [r0, #0x10]
    str     ip, [r0, #0x14]
    str     ip, [r0, #0x18]
    str     ip, [r0, #0x1c]
    add     r0, r0, #0x20
    subs    r3, r3, #1
    bne     L48
L74:
    and     r1, r2, #0x1f
    movs    r1, r1, lsr #2
    beq     L8c
L80:
    str     ip, [r0], #4
    subs    r1, r1, #1
    bne     L80
L8c:
    and     r2, r2, #3
L90:
    cmp     r2, #0
    bxeq    lr
    and     r1, ip, #0xff
L94:
    strb    r1, [r0], #1
    subs    r2, r2, #1
    bne     L94
    bx      lr
}
