// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
/* tail-call veneer: drop first arg, call func_ov015_021128e8(r1,r2) */
extern void func_ov015_021128e8(void);
asm void func_ov015_021128f8(void)
{
    ldr ip, [pc, #8]
    mov r0, r1
    mov r1, r2
    bx ip
    dcd func_ov015_021128e8
}
