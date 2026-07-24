// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
extern void func_020b66a8(void);
extern void *data_ov015_021147a4;
asm void func_ov015_02112c84(void)
{
    ldr ip, [pc, #4]
    ldr r1, [pc, #4]
    bx ip
    dcd func_020b66a8
    dcd data_ov015_021147a4
}
