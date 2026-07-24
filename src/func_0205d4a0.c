// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// Tail-call thunk: clear the two halfword fields at 0x34/0x36 unless field 0x10
// already holds 5, stash the two incoming args into 0x2c/0x30, then tail-call the
// pooled target with r1 forced to 5. Hand-asm because the trailing .word is a
// relocation to the real target (a wildcard) and mwccarm would otherwise emit a
// near `b` instead of the ROM's `ldr ip,[pc]; bx ip` pooled tail-call.
extern void func_0205cdf4(void);

asm void func_0205d4a0(void)
{
    ldr r3, [r0, #0x10]
    ldr ip, [pc, #0x1c]
    cmp r3, #5
    movne r3, #0
    strneh r3, [r0, #0x34]
    strneh r3, [r0, #0x36]
    str r1, [r0, #0x2c]
    mov r1, #5
    str r2, [r0, #0x30]
    bx ip
    dcd func_0205cdf4
}
